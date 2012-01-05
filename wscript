import os

srcdir ="."
bldir = "build"
version = "1.0"

def set_options(opt):
	opt.tool_options("compiler_cxx");

def configure(conf):
	conf.check_tool("compiler_cxx")
	conf.check_tool("node_addon")

def build(bld):

	bld.exec_command("mkdir -p uriparser; cd ../deps/uriparser && ./configure --with-pic --disable-test --disable-doc --disable-shared --prefix=%s && make clean install" % (bld.bdir + "/uriparser"))

	obj = bld.new_task_gen("cxx", "shlib", "node_addon")
	obj.target = "uriparser"
	obj.source = "src/node-uriparser.cc"
	obj.lib = ["uriparser"]
	obj.includes = [bld.bdir + "/uriparser/include/uriparser"]
	obj.libpath = [bld.bdir + "/uriparser/lib"]
	obj.cxxflags = ["-g"]
	obj.cflags = ["-g"]

def shutdown():
	if os.path.exists("build/default/uriparser.node"):
		os.system("cp build/default/uriparser.node ./bin")
