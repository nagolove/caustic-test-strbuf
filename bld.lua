return {
    {
        not_dependencies = {
            "lfs", "rlwr", "resvg",
        },
        artifact = "strbuf_test",
        kind = 'app',
        main = "strbuf-test.c",
        src = "src",
    },
}
