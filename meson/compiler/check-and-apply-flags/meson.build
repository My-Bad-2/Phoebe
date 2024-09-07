if get_variable('compile_settings_list', []).length() == 0
    warning(
        'Expected variable compile_settings_list not defined. Not processing compiler arguments.',
    )
else
    foreach entry : compile_settings_list
        add_project_arguments(
            entry.get('compiler').get_supported_arguments(entry.get('flags') + common_compile_flags),
            language: entry.get('lang'),
        )
    endforeach
endif

if get_variable('link_settings_list', []).length() == 0
    warning(
        'Expected variable link_settings_list not defined. Not processing linker arguments.',
    )
else
    foreach entry : link_settings_list
        supporting_link_flags = meson.get_external_property(
            'get_supported_link_arg_flags',
            [],
        )

        foreach flag : entry.get('flags') + common_link_flags
            if entry.get('compiler').has_multi_link_arguments(flag, supporting_link_flags)
                add_project_link_arguments(
                    flag,
                    language: entry.get('lang'),
                )
            endif
        endforeach
    endforeach
endif