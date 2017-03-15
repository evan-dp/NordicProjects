#include "nrf_dfu_handling_error.h"

#include "nrf_log.h"
#include "nrf_dfu_req_handler.h"

static nrf_dfu_ext_error_code_t m_last_error = NRF_DFU_EXT_ERROR_NO_ERROR;

nrf_dfu_res_code_t ext_error_set(nrf_dfu_ext_error_code_t error_code)
{
    m_last_error = error_code;

    return NRF_DFU_RES_CODE_EXT_ERROR;
}

nrf_dfu_ext_error_code_t ext_error_get()
{
    nrf_dfu_ext_error_code_t last_error = m_last_error;
    m_last_error = NRF_DFU_EXT_ERROR_NO_ERROR;

    return last_error;
}
