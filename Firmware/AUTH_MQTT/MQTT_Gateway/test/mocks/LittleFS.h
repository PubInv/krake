#pragma once
#include "Arduino.h"
#include "lfs.h"
#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

// ── RAM flash config ──────────────────────────────────────────────────
#define LFS_FLASH_SIZE   (256 * 1024)
#define LFS_BLOCK_SIZE   4096
#define LFS_BLOCK_COUNT  (LFS_FLASH_SIZE / LFS_BLOCK_SIZE)

inline uint8_t g_flash_buf[LFS_FLASH_SIZE];
inline lfs_t   g_lfs;
inline bool    g_lfs_mounted = false;

// ── Block device callbacks ────────────────────────────────────────────
inline int lfs_ram_read(const struct lfs_config* c, lfs_block_t block,
                        lfs_off_t off, void* buf, lfs_size_t size) {
    memcpy(buf, g_flash_buf + block * LFS_BLOCK_SIZE + off, size);
    return LFS_ERR_OK;
}
inline int lfs_ram_prog(const struct lfs_config* c, lfs_block_t block,
                        lfs_off_t off, const void* buf, lfs_size_t size) {
    memcpy(g_flash_buf + block * LFS_BLOCK_SIZE + off, buf, size);
    return LFS_ERR_OK;
}
inline int lfs_ram_erase(const struct lfs_config* c, lfs_block_t block) {
    memset(g_flash_buf + block * LFS_BLOCK_SIZE, 0xFF, LFS_BLOCK_SIZE);
    return LFS_ERR_OK;
}
inline int lfs_ram_sync(const struct lfs_config*) { return LFS_ERR_OK; }

inline const struct lfs_config g_lfs_cfg = {
    .context        = NULL,
    .read           = lfs_ram_read,
    .prog           = lfs_ram_prog,
    .erase          = lfs_ram_erase,
    .sync           = lfs_ram_sync,
    .read_size      = 16,
    .prog_size      = 16,
    .block_size     = LFS_BLOCK_SIZE,
    .block_count    = LFS_BLOCK_COUNT,
    .block_cycles   = 500,
    .cache_size     = 16,
    .lookahead_size = 16,
};

inline void lfs_ram_mount_fresh() {
    memset(g_flash_buf, 0xFF, LFS_FLASH_SIZE);
    lfs_format(&g_lfs, &g_lfs_cfg);
    lfs_mount(&g_lfs, &g_lfs_cfg);
    g_lfs_mounted = true;
}
inline void lfs_ram_unmount() {
    if (g_lfs_mounted) {
        lfs_unmount(&g_lfs);
        g_lfs_mounted = false;
    }
}

// ── File ──────────────────────────────────────────────────────────────
class File {
public:
    bool        _valid = false;
    bool        _isDir = false;
    std::string _path;
    std::string _name;

    // heap-allocated — address stays stable when File is copied
    std::shared_ptr<lfs_file_t> _file;
    std::shared_ptr<lfs_dir_t>  _dir;

    File() = default;

    explicit operator bool() const { return _valid; }
    bool        isDirectory() const { return _isDir; }
    const char* name()        const { return _name.c_str(); }

    String readString() {
        if (!_valid || _isDir || !_file) return String("");
        lfs_ssize_t sz = lfs_file_size(&g_lfs, _file.get());
        lfs_file_seek(&g_lfs, _file.get(), 0, LFS_SEEK_SET);
        std::string buf(sz, '\0');
        lfs_file_read(&g_lfs, _file.get(), &buf[0], sz);
        return String(buf.c_str());
    }

    void print(const char* s) {
        if (_valid && !_isDir && _file)
            lfs_file_write(&g_lfs, _file.get(), s, strlen(s));
    }

    void close() {
        if (!_valid) return;
        if (_isDir  && _dir)  lfs_dir_close (&g_lfs, _dir.get());
        if (!_isDir && _file) lfs_file_close(&g_lfs, _file.get());
        _valid = false;
    }

    File openNextFile() {
        if (!_valid || !_isDir || !_dir) return File{};
        lfs_info info;
        while (true) {
            int res = lfs_dir_read(&g_lfs, _dir.get(), &info);
            if (res <= 0) return File{};
            if (strcmp(info.name, ".") == 0 ||
                strcmp(info.name, "..") == 0) continue;

            std::string childPath = _path + "/" + info.name;
            File f;
            f._name = info.name;
            f._path = childPath;

            if (info.type == LFS_TYPE_DIR) {
                f._isDir = true;
                f._dir   = std::make_shared<lfs_dir_t>();
                f._valid = (lfs_dir_open(&g_lfs, f._dir.get(),
                                         childPath.c_str()) == 0);
            } else {
                f._isDir = false;
                f._file  = std::make_shared<lfs_file_t>();
                f._valid = (lfs_file_open(&g_lfs, f._file.get(),
                                          childPath.c_str(),
                                          LFS_O_RDONLY) == 0);
            }
            return f;
        }
    }
};

// ── LittleFS ──────────────────────────────────────────────────────────
struct LittleFSClass {

    File open(const char* path, const char* mode = "r") {
        File f;
        f._path = path;
        std::string p(path);
        size_t slash = p.rfind('/');
        f._name = (slash == std::string::npos) ? p : p.substr(slash + 1);

        lfs_info info;
        if (lfs_stat(&g_lfs, path, &info) == 0 &&
            info.type == LFS_TYPE_DIR) {
            f._isDir = true;
            f._dir   = std::make_shared<lfs_dir_t>();
            f._valid = (lfs_dir_open(&g_lfs, f._dir.get(), path) == 0);
            return f;
        }

        int flags = (mode[0] == 'w')
                    ? LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC
                    : LFS_O_RDONLY;
        f._isDir = false;
        f._file  = std::make_shared<lfs_file_t>();
        f._valid = (lfs_file_open(&g_lfs, f._file.get(), path, flags) == 0);
        return f;
    }

    bool mkdir(const char* path) {
        return lfs_mkdir(&g_lfs, path) == 0;
    }

    bool remove(const char* path) {
        return lfs_remove(&g_lfs, path) == 0;
    }
};

inline LittleFSClass LittleFS;