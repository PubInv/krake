m#include <unity.h>
#include "gateway_device.h"   // found via -I lib/gateway_core

// ── Helpers ───────────────────────────────────────────────────────────

// Builds a valid JSON device file the way saveDevice() would write it
static void write_device_file(const char* id, const char* name,
                               const char* type, int status,
                               unsigned long nonce) {
    char path[64];
    snprintf(path, sizeof(path), "/devices/dev_%s", id);

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"id\":\"%s\",\"name\":\"%s\",\"type\":\"%s\","
        "\"status\":%d,\"lastNonce\":%lu,"
        "\"firstSeen\":1000,\"lastSeen\":2000,"
        "\"messageCount\":3,\"permPing\":false,\"key\":\"\"}",
        id, name, type, status, nonce);

    lfs_file_t f;
    lfs_file_open(&g_lfs, &f, path, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    lfs_file_write(&g_lfs, &f, buf, strlen(buf));
    lfs_file_close(&g_lfs, &f);
}

// ── setUp / tearDown ──────────────────────────────────────────────────

void setUp() {
    lfs_ram_mount_fresh();              // blank formatted flash per test
    lfs_mkdir(&g_lfs, "/devices");     // pre-create the devices directory
}

void tearDown() {
    lfs_ram_unmount();
}

// ── Tests ─────────────────────────────────────────────────────────────

// loadDevices() on empty directory → no devices loaded, no crash
void test_load_empty_directory() {
    GatewayDevice gd;
    gd.loadDevices();
    TEST_ASSERT_EQUAL(0, (int)gd.devices.size());
}

// loadDevices() creates /devices dir when it doesn't exist yet
void test_load_creates_devices_dir_if_missing() {
    lfs_remove(&g_lfs, "/devices");    // remove it so loadDevices must create it

    GatewayDevice gd;
    gd.loadDevices();

    lfs_info info;
    int res = lfs_stat(&g_lfs, "/devices", &info);
    TEST_ASSERT_EQUAL(0, res);
    TEST_ASSERT_EQUAL(LFS_TYPE_DIR, (int)info.type);
}

// loadDevices() correctly parses a JSON device file
void test_load_reads_one_device() {
    write_device_file("sensor_01", "Temp Sensor", "sensor", DEV_APPROVED, 42);

    GatewayDevice gd;
    gd.loadDevices();

    TEST_ASSERT_EQUAL(1, (int)gd.devices.size());
    TEST_ASSERT_TRUE(gd.devices.count("sensor_01") > 0);

    const Device& d = gd.devices["sensor_01"];
    TEST_ASSERT_EQUAL_STRING("sensor_01",   d.id.c_str());
    TEST_ASSERT_EQUAL_STRING("Temp Sensor", d.name.c_str());
    TEST_ASSERT_EQUAL_STRING("sensor",      d.type.c_str());
    TEST_ASSERT_EQUAL(DEV_APPROVED,         (int)d.status);
    TEST_ASSERT_EQUAL(42,                   (int)d.lastNonce);
    TEST_ASSERT_FALSE(d.has_pending);
}

// loadDevices() loads multiple devices in one pass
void test_load_reads_multiple_devices() {
    write_device_file("node_A", "Node A", "relay",  DEV_PENDING,  1);
    write_device_file("node_B", "Node B", "sensor", DEV_APPROVED, 9);

    GatewayDevice gd;
    gd.loadDevices();

    TEST_ASSERT_EQUAL(2, (int)gd.devices.size());
    TEST_ASSERT_TRUE(gd.devices.count("node_A") > 0);
    TEST_ASSERT_TRUE(gd.devices.count("node_B") > 0);
}

// saveDevice() writes a file that actually exists on the FS
void test_save_creates_file() {
    Device dev;
    dev.id           = "relay_01";
    dev.name         = "Main Relay";
    dev.type         = "relay";
    dev.status       = DEV_APPROVED;
    dev.lastNonce    = 7;
    dev.firstSeen    = 100;
    dev.lastSeen     = 200;
    dev.messageCount = 0;
    dev.permPing     = false;
    dev.keySet       = false;

    GatewayDevice gd;
    gd.saveDevice(dev);

    lfs_info info;
    int res = lfs_stat(&g_lfs, "/devices/dev_relay_01", &info);
    TEST_ASSERT_EQUAL_MESSAGE(0, res, "File should exist after saveDevice()");
    TEST_ASSERT_TRUE(info.size > 0);
}

// save then load round-trip preserves all fields
void test_roundtrip_save_load() {
    Device dev;
    dev.id           = "node_A";
    dev.name         = "Node Alpha";
    dev.type         = "relay";
    dev.status       = DEV_PENDING;
    dev.lastNonce    = 99;
    dev.firstSeen    = 10;
    dev.lastSeen     = 20;
    dev.messageCount = 5;
    dev.permPing     = true;
    dev.keySet       = false;

    GatewayDevice gd;
    gd.saveDevice(dev);
    gd.devices.clear();
    gd.loadDevices();

    TEST_ASSERT_EQUAL(1, (int)gd.devices.size());
    const Device& d = gd.devices["node_A"];
    TEST_ASSERT_EQUAL_STRING("Node Alpha", d.name.c_str());
    TEST_ASSERT_EQUAL_STRING("relay",      d.type.c_str());
    TEST_ASSERT_EQUAL(DEV_PENDING,         (int)d.status);
    TEST_ASSERT_EQUAL(99,                  (int)d.lastNonce);
    TEST_ASSERT_EQUAL(5,                   d.messageCount);
    TEST_ASSERT_TRUE(d.permPing);
}

// removeDevice() deletes the file from flash
void test_remove_deletes_file() {
    write_device_file("sensor_01", "Temp Sensor", "sensor", DEV_PENDING, 0);

    GatewayDevice gd;
    gd.removeDevice("sensor_01");

    lfs_info info;
    int res = lfs_stat(&g_lfs, "/devices/dev_sensor_01", &info);
    TEST_ASSERT_NOT_EQUAL(0, res);   // file should no longer exist
}

// safeFilename() replaces all illegal characters
void test_safe_filename_special_chars() {
    GatewayDevice gd;
    String result = gd.safeFilename("home/topic:name*val");
    TEST_ASSERT_EQUAL_STRING("home_topic_name_val", result.c_str());
}

// ── Runner ────────────────────────────────────────────────────────────
int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_load_empty_directory);
    RUN_TEST(test_load_creates_devices_dir_if_missing);
    RUN_TEST(test_load_reads_one_device);
    RUN_TEST(test_load_reads_multiple_devices);
    RUN_TEST(test_save_creates_file);
    RUN_TEST(test_roundtrip_save_load);
    RUN_TEST(test_remove_deletes_file);
    RUN_TEST(test_safe_filename_special_chars);
    UNITY_END();
    return 0;
}