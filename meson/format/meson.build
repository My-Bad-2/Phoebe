###############################################
# Automated Code Formatting with Clang-Format #
###############################################

# [Optional] specify files and directories to exclude from the build
clangformat_excludes = get_variable('clangformat_excludes', [])

# This variable should be a string, which is a comma-separated list of directories
# to exclude from formatting. The -e argument is used to indicate excludes in the script.
if clangformat_excludes.length() > 0
	clangformat_excludes_processed = ''
	foreach entry : clangformat_excludes
		clangformat_excludes_processed += entry + ','
	endforeach
	clangformat_excludes_processed = ['-e', clangformat_excludes_processed]
else
	clangformat_excludes_processed = []
endif

# Target files and folders to include in clang-format analysis
clangformat_default_targets = [
	meson.project_source_root() / 'kernel',
	meson.project_source_root() / 'libc',
]

# Override this variable to supply your own list of targets
clangformat_targets = get_variable('clangformat_targets', clangformat_default_targets)
# Define this variable to add new targets to the default list
clangformat_additional_targets = get_variable('clangformat_additional_targets', [])

# Convert into expected format for script: CSV
clangformat_targets_processed = ''
foreach entry : clangformat_targets + clangformat_additional_targets
	clangformat_targets_processed += entry + ','
endforeach

# File types to be included in cppcheck analysis
clangformat_default_file_types = [
	'*.c',
	'*.h',
	'*.cpp',
	'*.hpp'
]

# Override this variable to supply your own list of file types
clangformat_file_types = get_variable('clangformat_file_types', clangformat_default_file_types)
clangformat_additional_file_types = get_variable('clangformat_additional_file_types', [])

# Convert into expected format for script: CSV
clangformat_file_types_processed = ''
foreach entry : clangformat_file_types + clangformat_additional_file_types
	clangformat_file_types_processed += entry + ','
endforeach

## Default arguments used for clang format processing script
clangformat_default_args = [
	# Optional exclude directories (separated by commas)
	clangformat_excludes_processed,
	# Directories to format files in
	clangformat_targets_processed,
	# File types to include in formatting (separated by commas)
	clangformat_file_types_processed,
]

# Override this variable to supply your own raw arguments to the script
clangformat_args = get_variable('clangformat_args', clangformat_default_args)

# Clang-format run target will be unavailable if the program isn't found
clangformat = find_program('clang-format', required: false, disabler: true)

# If you want to implement a custom clang-format target, you can reference
# This files object to gain access to the script
clangformat_script = files('format.sh')

if clangformat.found()
	run_target('format',
		command: [
			clangformat_script,
			# Supply arguments
			clangformat_args
		]
	)

	run_target('format-patch',
		command: [
			clangformat_script,
			# Enable patch file,
			'-p',
			# Supply arguments
			clangformat_args
		]
	)
endif

cp = find_program('cp')

run_target('update-format-rules',
	command: [
		cp,
		meson.current_source_dir() + '/.clang-format',
		meson.project_source_root()
	]
)