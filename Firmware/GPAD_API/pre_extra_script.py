Import("env")

cpp_defines = [
    ("COMPANY_NAME", "PubInv "),   # For the Broker ID for MQTT 
    ("PROG_NAME", "GPAD_API "),    # This program
    ("FIRMWARE_VERSION", "0.55 "), # pr 501
    ("LittleFS_VERSION", "0.1.6 "), # pr 501
    ("MODEL_NAME", "KRAKE_"), 
    ("LICENSE", "GNU Affero General Public License, version 3 "),
    ("ORIGIN", "US"),
]

stringify_lambda = lambda macro: (macro[0], env.StringifyMacro(macro[1]))

env.Append(CPPDEFINES=map(stringify_lambda, cpp_defines))
