{
    "targets": [
        {
            "target_name": "uriparser",
            "dependencies": [
            ],
            "sources": [
                "src/node-uriparser.cc"
            ],
            "include_dirs": [
                "<(module_root_dir)/deps/uri-parser"
            ],
            "cflags": [
                "-g"
            ],
        },
        {
            "target_name": "after_build",
            "type": "none",
            "dependencies": [
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
