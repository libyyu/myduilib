if is_plat("windows") then
	includes(
		--'TestApp1',
		'Client',
		'Server',
		'DuiLibApp',
		'LuaDuiLibApp',
		--'QQDemo',
		--'RichListDemo',
		'WebService',
		--'ScrCapture',
		--'360SafeDemo',
		--'GameDemo',
		'MyControls',
		'YMusic'
	)
else
	if has_config("boost_dir") then
		includes(
			'WebService'
		)
	end
end