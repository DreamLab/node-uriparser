{
    "targets": [
        {
            "target_name": "uriparser",
            "dependencies": ["liburiparser"],
            "sources": [
                "src/node-uriparser.cc"
            ],
            "include_dirs": [
                "<(module_root_dir)/build/uriparser/include/uriparser"
            ],
            "cflags": [
                "-g"
            ],
            "libraries": [
                "<(module_root_dir)/build/uriparser/lib/liburiparser.a"
            ]
        },
        {
            "target_name": "liburiparser",
            "type": "none",
            "actions": [{
                "action_name": "build",
                "inputs": [""],
                "outputs": [""],
                "action": ["sh", "build_uriparser.sh"]
            }]
        }
    ]
}
