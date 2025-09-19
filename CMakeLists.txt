zephyr_library()
zephyr_library_sources(
    src/display/status_screen.c
    #   src/display/widgets/hello_widget.c
    src/display/widgets/batt_net_widget.c
)
zephyr_include_directories(include)
