###############################
# Clang-tidy Analysis Support #
###############################
# Clang-tidy run target will be unavailable if the program isn't found
clangtidy = find_program('clang-tidy', required: false, disabler: true)

# If you want to implement a custom clang-tidy target, you can reference
# This files object to gain access to the script
clangtidy_script = files('clang-tidy.sh')

if clangtidy.found()
	if get_variable('clangtidy_files', []).length() == 0
		warning('clangtidy_files list not defined before including module, so the target is disabled.')
	else
		run_target('clang-tidy',
			command: [
				clangtidy_script,
				# Optional user overrides
				get_variable('clangtidy_options', ''),
				# Files,
				clangtidy_files
			],
		)
		run_target('clang-tidy-fix',
			command: [
				clangtidy_script,
				# Optional user overrides
				get_variable('clangtidy_options', ''),
				'--fix-errors', '--fix-notes',
				# Files,
				clangtidy_files
			],
		)
	endif
endif