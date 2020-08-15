--mac下使用系统zlib
if not is_plat("iphoneos") and not is_plat("macosx") then
	includes('zlib')
end 

if is_plat("windows") then
	includes("StackDump")
end

includes(
	'MainThreadTask',
	'lua51',
	'protobuf',
	'protos',
	'json',
	'taglib',
	'taglib_plugin',
	'http',
	'FLib',
	'sqlite3',
	'sqlite3cpp',
	'database'
)

if has_config("boost_dir") then
	includes('network', 'WebService')
	if is_plat("windows") then
		includes('Agent')
	end 
	includes('operationQueue')
	includes('play_plugin')
end
includes('vlc', 'luaext', 'lpeg')