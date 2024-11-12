load("//:shcustomrule.bzl","gen_measurements_rule")


exports_files(["target/average-1.0.0-SNAPSHOT.jar"], visibility = ["//src/cpp:__pkg__"])
exports_files(["create_measurements.sh"], visibility = ["//src/cpp:__pkg__"])

filegroup(
    name = "measurement",
    srcs = ["measurements.txt"],
    visibility = ["//visibility:public"],
)

# genrule(
#   name = "sh_measurement",
#   srcs = ["target/average-1.0.0-SNAPSHOT.jar"],
#   outs = ["measurements.txt"],
#   tools = ["create_measurements.sh"],
#   cmd = """
#     bash $(location create_measurements.sh) 1000000000 $(location target/average-1.0.0-SNAPSHOT.jar) && \\
#     cp measurements.txt $(location measurements.txt)
#   """,
#   visibility = ["//visibility:public"],
# )

# genrule(
#     name = "jar_measurement",
#     srcs = ["target/average-1.0.0-SNAPSHOT.jar"],
#     outs = ["measurements.txt"],
#     cmd = """
#         java --class-path $(location target/average-1.0.0-SNAPSHOT.jar) dev.morling.onebrc.CreateMeasurements 1000000000 && \\
#         cp measurements.txt $(location measurements.txt)
#     """,
#     visibility = ["//visibility:public"],
# )

gen_measurements_rule(
    name = "rule_measurement",
    jar = "//:target/average-1.0.0-SNAPSHOT.jar",
    visibility = ["//visibility:public"],
)