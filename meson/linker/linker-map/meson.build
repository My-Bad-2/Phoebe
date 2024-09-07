if c_compiler.has_link_argument('-Wl,-map,test.map')
    map_file = '-Wl,-map,@0@.map'
elif c_compiler.has_link_argument('-Wl,-Map,test.map')
    map_file = '-Wl,-Map,@0@.map'
else
    warning('Tool chain does not support Map file generation')
    map_file = []
endif