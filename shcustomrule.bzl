def _gen_measurements_rule_impl(ctx):
    jar = ctx.file.jar
    output = ctx.outputs.out

    # Run the Java command and copy the generated file to the output location
    ctx.actions.run_shell(
        inputs = [jar],
        outputs = [output],
        command = """
            java --class-path {jar_path} \\
            dev.morling.onebrc.CreateMeasurements 1000000000 && \\
            cp measurements.txt {output_path}
        """.format(jar_path=jar.path, output_path=output.path),
        env = {"PATH": "/home/mfw-150/.sdkman/candidates/java/21.0.5-graal/bin:/usr/bin"}
    )

gen_measurements_rule = rule(
    implementation = _gen_measurements_rule_impl,
    attrs = {
        "jar": attr.label(mandatory = True, allow_single_file = True),
    },
    outputs = {"out": "measurements.txt"},
)
