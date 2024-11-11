filegroup(
    name = "measurement",
    srcs = ["measurements.txt"],
    visibility = ["//visibility:public"],
)

genrule(
  name = "sh_measurement",
  srcs = ["target/average-1.0.0-SNAPSHOT.jar"],
  outs = ["measurements.txt"],
  tools = ["create_measurements.sh"],
  cmd = "bash $(location create_measurements.sh) 1000000000 $(location target/average-1.0.0-SNAPSHOT.jar) && cp measurements.txt $(location measurements.txt)",
  visibility = ["//visibility:public"],
)