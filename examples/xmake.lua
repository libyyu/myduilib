includes("test_flib", "test_http_plugin", "test_playcenter", "test_lua")

if is_plat("windows") then
	includes(
        'test_webservice'
        ,'test_play_plugin'
	)
else
	if has_config("boost_dir") then
		includes(
			'test_webservice'
		)
	end
end