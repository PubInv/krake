Import("env")

# Check which platform is currently building
platform = env.get("PIOPLATFORM")

if platform == "native":
    # Build ONLY gateway_device.cpp
    # "-<*>" removes everything, "+<gateway_device.cpp>" adds just that one
    env.Replace(SRC_FILTER=["-<*>", "+<gateway_device.cpp>"])

else:
    # Build everything for ESP32 or any other platform
    env.Replace(SRC_FILTER=["+<*>"])
