/**
  ******************************************************************************
  * @file    appgas.c
  * @author  ZKRT
  * @version V1.0
  * @date    5-January-2018
  * @brief
  *          + (1) init
  *
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "appgas.h"
#include "baoshian.h"
#include "guorui.h"
#include "zkrt_gas.h"
#include "dev_handle.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
hb_gas_st* dev_gasinfo;
u8 which_gas_manu;   //使用哪个厂家的气体探测器
u8 gasvalue_need_update;  //数据需要更新


//zkrt gas type and gas unit type mapping for baoshian or other manufactury
//gas type map -- index is zkrt type, value is baoshian type or guorui type, index order: baoshian,guorui
//gas type map array
const u8 gas_type_map[max_zkst][2] =
{
    {0xff, 0xff}, {co_bsast, co_grst}, {h2s_bsast, h2s_grst}, {nh3_bsast, nh3_grst}, {0xff, co2_grst},
    {cl2_bsast, cl2_grst}, {so2_bsast, so2_grst}, {0xff, voc_grst}, {ch4_bsast, ch4_grst}, {o2_bsast, o2_grst}
};
//gas unit type map array -- baoshian,guorui
const u8 gas_unit_map[max_zkunit][2] =
{
    {umoldiviedmol_bsaunit, 0xff}, {VOLpercent_bsaunit, VOLpercent_grunit}, {LELpercent_bsaunit, LELpercent_grunit}, {PPM_bsaunit, PPM_grunit},
    {mgdivied10cube_bsaunit, 0xff}, {percent_bsaunit, percent_grunit}, {tenpowernegative6_bsaunit, 0xff},
    {0xff, PPB_grunit}, {0xff, permillage_grunit}, {0xff, mgdiviedm3_gruint}, {0xff, mgdiviedL_grunit}
};
/* Private functions ---------------------------------------------------------*/
static void app_gas_value_update(void);
//protocol fun
static char none_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char getchnum_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char getchinfo_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen);
typedef char (*GASPTOCOL_FUN)(void*, void*, u8, u8*);
const GASPTOCOL_FUN gasptcol_fun[] =
{
    none_gasptf,
    getchnum_gasptf,
    getchinfo_gasptf
};

/**
  * @brief  appcan_init
  * @param  None
  * @retval None
  */
void appgas_init(void)
{
    //parameter init
    dev_gasinfo = &zkrt_devinfo.devself->gas;
    //
    baoshian_init();
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void appgas_prcs(void)
{
    if(gr_handle.gas_online_flag==0)
        baoshian_prcs();
    app_gas_value_update();
}
/**
  * @brief
  * @param  None
  * @retval None
  */
u8 appgas_message_handle(zkrt_packet_t* recvpack, u8* respond_data, u8* reslen)
{
    u8 ret =NOTNEED_RETRUN;
    u8 respacket_len;
    send_comspec_plst *comspec = (send_comspec_plst*)recvpack->data;
    respond_comspec_plst *rcomspec = (respond_comspec_plst*)(respond_data+sizeof(zkrtpacket_header));
    zkrtpacket_header* resheader = (zkrtpacket_header*)respond_data;
    zkrtpacket_tailer* restailer;

    if(recvpack->command != ZK_COMMAND_SPECIFIED)
        return NOTNEED_RETRUN;

    if((comspec->control_num <=NONE_GASCN)||(comspec->control_num >=MAX_GASCN))
        return NOTNEED_RETRUN;

    //parse and pack respond packet
    ret = gasptcol_fun[recvpack->data[0]](recvpack->data, rcomspec, recvpack->length, &respacket_len);
    //pack respond_data header and tailer and so on so forth
    if(ret==NEED_RETRUN)
    {
        memcpy(resheader, recvpack, sizeof(zkrtpacket_header));
        resheader->cmd = UAV_TO_APP;
        resheader->length = respacket_len;
        restailer = (zkrtpacket_tailer*)(rcomspec+respacket_len);
        restailer->end_code = _END_CODE;
        restailer->crc = crc_calculate(respond_data, respacket_len+ZK_HEADER_LEN);
        *reslen = respacket_len+ZK_FIXED_LEN;
    }
    return ret;
}
/**
  * @brief
  * @param  None
  * @retval None
  */
static void app_gas_value_update(void)
{
    int i;
    if(gr_handle.gas_online_flag)
    {
        if(gr_handle.gas_update_flag)
        {
            which_gas_manu = GASTYPE_MAP_GR_INDEX;
            gr_handle.gas_update_flag = 0;
            dev_gasinfo->ch_num = guorui_gas_info.ch_num;
            dev_gasinfo->ch_status = guorui_gas_info.ch_status;
            memcpy(dev_gasinfo->gas_value, guorui_gas_info.gas_value, sizeof(guorui_gas_info.gas_value));
        }
    }
    else if(bsa_prcs_handle.gas_online_flag)
    {
        if(bsa_prcs_handle.gas_update_flag)
        {
            which_gas_manu = GASTYPE_MAP_BSA_INDEX;
            bsa_prcs_handle.gas_update_flag = 0;
            dev_gasinfo->ch_num = bsa_dev_info.bsa_fixed_dev.channel_num;
            dev_gasinfo->ch_status = bsa_dev_info.ch_status;
            for(i=0; i<dev_gasinfo->ch_num; i++)
            {
                dev_gasinfo->gas_value[i] = bsa_chgas_info[i].gas_value;
            }
        }
    }
}
/////////////////////////////////////////////////////////////////// zkrt gas type special
u8 manu_gastype2zkrt(u8 manu, u8 type)
{
    int i;
    if((manu >1)||(type==0xff))
        return 0xff;
    for(i=0; i< max_zkst; i++)
    {
        if(gas_type_map[i][manu] == type)
            return i;
    }
    return 0xff;
}
u8 manu_gasunittype2zkrt(u8 manu, u8 type)
{
    int i;
    if((manu >1)||(type==0xff))
        return 0xff;
    for(i=0; i< max_zkst; i++)
    {
        if(gas_unit_map[i][manu] == type)
            return i;
    }
    return 0xff;
}
///////////////////////////////////////////////////////////////////////////////app gas protocl funciton
//
static char none_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
    return NOTNEED_RETRUN;
}
//
static char getchnum_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
    u8 res = S_Success_Gas;
    send_comspec_plst *s = (send_comspec_plst*)sdata;
    respond_comspec_plst *r = (respond_comspec_plst*)rdata;
    rgetchnum_gas_comspecplst *rother = (rgetchnum_gas_comspecplst*)r->other_data;

    if(bsa_prcs_handle.gas_online_flag ==0)
    {
        res = S_FailNoDev_Gas;
    }

    rother->ch_num = dev_gasinfo->ch_num;

    //respond header
    r->control_num = s->control_num;
    r->status = res;
    *rlen = sizeof(rgetchnum_gas_comspecplst) + RES_HEADER_LEN;

    return NEED_RETRUN;
}
//
static char getchinfo_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
    u8 res = S_Success_Gas;
    send_comspec_plst *s = (send_comspec_plst*)sdata;
    respond_comspec_plst *r = (respond_comspec_plst*)rdata;
    getchinfo_gas_comspecplst *sother = (getchinfo_gas_comspecplst*)s->other_data;
    rgetchinfo_gas_comspecplst *rother = (rgetchinfo_gas_comspecplst*)r->other_data;

    if(bsa_prcs_handle.gas_online_flag ==0)
        res = S_FailNoDev_Gas;

    if(sother->ch >= dev_gasinfo->ch_num)
        res = S_FailNoChannel_Gas;

    if(res==S_Success_Gas)
    {
        if(sother->ch <BSA_GAS_NUM)
        {
            rother->ch = bsa_chgas_info[sother->ch].channel;
            rother->value = bsa_chgas_info[sother->ch].gas_value;
            rother->sensor_type = manu_gastype2zkrt(GASTYPE_MAP_BSA_INDEX,
                                                    bsa_chgas_info[sother->ch].sensor_type);
            rother->unit_type = manu_gasunittype2zkrt(GASTYPE_MAP_BSA_INDEX,
                                bsa_chgas_info[sother->ch].unit_type);
            rother->range = bsa_chgas_info[sother->ch].measure_range;
        }
        else
            res = S_FailNoChannel_Gas; //zkrt_todo we will support 8 baoshian gas type in the future
    }
    //respond header
    r->control_num = s->control_num;
    r->status = res;
    *rlen = sizeof(rgetchinfo_gas_comspecplst) + RES_HEADER_LEN;

    return NEED_RETRUN;
}
