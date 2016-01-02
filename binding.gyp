{
    "targets": [
        {
            "target_name": "uriparser",
            "dependencies": [
                "libngx_url_parser"
            ],
            "sources": [
                "src/node-uriparser.cc"
            ],
            "include_dirs": [
                "<(module_root_dir)/deps/ngx_url_parser"
            ],
            "cflags": [
                "-g"
            ],
            "libraries": [
                "<(module_root_dir)/deps/ngx_url_parser/lib/libngx_url_parser.a"
            ]
        },
        {
            "target_name": "libngx_url_parser",
            "type": "none",
            "actions": [
                {
                    "action_name": "build",
                    "inputs": [""],
                    "outputs": [""],
                    "action": ["sh", "build_ngx_url_parser.sh"]
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
