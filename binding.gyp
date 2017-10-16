{
    "targets": [
        {
            "target_name": "uriparser",
            "dependencies": [
                "libparsers"
            ],
            "sources": [
                "src/node-uriparser.cc"
            ],
            "include_dirs": [
                "<(module_root_dir)/build/ngx_url_parser/include/ngx_url_parser",
                "<!(node -e \"require('nan')\")"

            ],
            "cflags": [
                "-O3"
            ],
            "libraries": [
                "<(module_root_dir)/build/ngx_url_parser/lib/libngx_url_parser.a",
            ]
        },
        {
            "target_name": "libparsers",
            "type": "none",
            "actions": [
                {
                    "action_name": "build",
                    "inputs": [""],
                    "outputs": [""],
                    "action": ["sh", "build_deps.sh"]
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
                    "action_name": "copy",
                    "inputs": [
                        "<@(PRODUCT_DIR)/uriparser.node"
                    ],
                    "outputs": [
                        "<(module_root_dir)/bin/uriparser.node"
                    ],
                    "action": ["cp", "<@(PRODUCT_DIR)/uriparser.node", "<(module_root_dir)/bin/uriparser.node"]
                }
            ]
        }
    ]
}
