{
    "targets": [
        {
            "target_name": "uriparser",
            "dependencies": [
                "liburiparser"
            ],
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
            "actions": [
                {
                    "action_name": "build",
                    "inputs": [""],
                    "outputs": [""],
                    "action": ["sh", "build_uriparser.sh"]
                }
            ]
        },
        {
            "target_name": "after_build",
            "type": "none",
            "dependencies": [
                "uriparser"
            ],
            "actions": [
                {
                    "action_name": "symlink",
                    "inputs": [
                        "<@(PRODUCT_DIR)/uriparser.node"
                    ],
                    "outputs": [
                        "<(module_root_dir)/bin/uriparser.node"
                    ],
                    "action": ["ln", "-s", "<@(PRODUCT_DIR)/uriparser.node", "<(module_root_dir)/bin/uriparser.node"]
                }
            ]
        }
    ]
}
