import Options
import pprint
import sys

srcdir ="."
bldir = "build"
version = "1.0"

def set_options(opt):
    opt.add_option('-b', '--build-uri-parser', action='store_false', default=True, dest='buildUri', help='build with bundled uriparser');
    opt.tool_options("compiler_cxx")

def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.check_tool("node_addon")

def do_clean():
    for arg in sys.argv:
        if arg == 'clean':
            return True

    return False

def build(bld):
    if Options.options.buildUri == True and do_clean() == False:
        bld.exec_command("mkdir -p uriparser; cd ../deps/uriparser && ./autogen.sh && ./configure --with-pic --disable-test --disable-doc --disable-shared --prefix=%s && make clean install" % (bld.bdir + "/uriparser"))
        bld.exec_command("cd ../deps/ngx_url_parser && make static-lib")

    obj = bld.new_task_gen("cxx", "shlib", "node_addon")
    obj.target = "uriparser"
    obj.source = "src/node-uriparser.cc"
    obj.lib = ["uriparser", "ngx_url_parser"]
    obj.includes = [bld.bdir + "/uriparser/include/uriparser", "deps/ngx_url_parser/"]
    obj.libpath = [bld.bdir + "/uriparser/lib", '../deps/ngx_url_parser/lib']
    obj.cxxflags = ["-g"]
    obj.cflags = ["-g"]
