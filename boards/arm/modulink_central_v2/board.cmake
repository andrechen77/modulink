# see also https://github.com/zmkfirmware/zmk/blob/main/app/boards/arm/nice_nano/board.cmake

board_runner_args(nrfjprog "--nrf-family=NRF52" "--softreset")
include(${ZEPHYR_BASE}/boards/common/uf2.board.cmake)
include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
# include(${ZEPHYR_BASE}/boards/common/nrfutil.board.cmake)
# include(${ZEPHYR_BASE}/boards/common/openocd-nrf5.board.cmake)