import Options
import pprint
import sys

srcdir ="."
bldir = "build"
version = "1.0"

def set_options(opt):
    opt.add_option('-b', '--build-uri-parser', action='store_false', default=True, dest='buildUri', help='build with bundled deps');
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
        bld.exec_command("mkdir -p ngx_url_parser; cd ../deps/ngx_url_parser && ./autogen.sh && ./configure --with-pic  --disable-shared --prefix=%s && make clean install" %(bld.bdir + "/ngx_url_parser"))

    obj = bld.new_task_gen("cxx", "shlib", "node_addon")
    obj.target = "uriparser"
    obj.source = "src/node-uriparser.cc"
    obj.lib = [ "ngx_url_parser"]
    obj.includes = [bld.bdir + "/ngx_url_parser/include/ngx_url_parser"]
    obj.libpath = [bld.bdir + '/ngx_url_parser/lib']
    obj.cxxflags = ["-O3"]
    obj.cflags = ["-O3"]
