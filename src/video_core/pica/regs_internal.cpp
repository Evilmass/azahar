// Copyright Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <algorithm>
#include <iterator>
#include <utility>

#include "common/common_types.h"
#include "video_core/pica/regs_internal.h"

namespace Pica {

static constexpr std::pair<u16, const char*> register_names[] = {
    {0x010, "GPUREG_FINALIZE"},
    {0x020, "GPUREG_IRQ_CMP"},
    {0x020, "GPUREG_IRQ_MASK"},
    {0x034, "GPUREG_IRQ_AUTOSTOP"},

    {0x040, "GPUREG_FACECULLING_CONFIG"},
    {0x041, "GPUREG_VIEWPORT_WIDTH"},
    {0x042, "GPUREG_VIEWPORT_INVW"},
    {0x043, "GPUREG_VIEWPORT_HEIGHT"},
    {0x044, "GPUREG_VIEWPORT_INVH"},

    {0x047, "GPUREG_FRAGOP_CLIP"},
    {0x048, "GPUREG_FRAGOP_CLIP_DATA0"},
    {0x049, "GPUREG_FRAGOP_CLIP_DATA1"},
    {0x04A, "GPUREG_FRAGOP_CLIP_DATA2"},
    {0x04B, "GPUREG_FRAGOP_CLIP_DATA3"},

    {0x04D, "GPUREG_DEPTHMAP_SCALE"},
    {0x04E, "GPUREG_DEPTHMAP_OFFSET"},
    {0x04F, "GPUREG_SH_OUTMAP_TOTAL"},
    {0x050, "GPUREG_SH_OUTMAP_O0"},
    {0x051, "GPUREG_SH_OUTMAP_O1"},
    {0x052, "GPUREG_SH_OUTMAP_O2"},
    {0x053, "GPUREG_SH_OUTMAP_O3"},
    {0x054, "GPUREG_SH_OUTMAP_O4"},
    {0x055, "GPUREG_SH_OUTMAP_O5"},
    {0x056, "GPUREG_SH_OUTMAP_O6"},

    {0x061, "GPUREG_EARLYDEPTH_FUNC"},
    {0x062, "GPUREG_EARLYDEPTH_TEST1"},
    {0x063, "GPUREG_EARLYDEPTH_CLEAR"},
    {0x064, "GPUREG_SH_OUTATTR_MODE"},
    {0x065, "GPUREG_SCISSORTEST_MODE"},
    {0x066, "GPUREG_SCISSORTEST_POS"},
    {0x067, "GPUREG_SCISSORTEST_DIM"},
    {0x068, "GPUREG_VIEWPORT_XY"},

    {0x06A, "GPUREG_EARLYDEPTH_DATA"},

    {0x06D, "GPUREG_DEPTHMAP_ENABLE"},
    {0x06E, "GPUREG_RENDERBUF_DIM"},
    {0x06F, "GPUREG_SH_OUTATTR_CLOCK"},

    {0x080, "GPUREG_TEXUNIT_CONFIG"},
    {0x081, "GPUREG_TEXUNIT0_BORDER_COLOR"},
    {0x082, "GPUREG_TEXUNIT0_DIM"},
    {0x083, "GPUREG_TEXUNIT0_PARAM"},
    {0x084, "GPUREG_TEXUNIT0_LOD"},
    {0x085, "GPUREG_TEXUNIT0_ADDR1"},
    {0x086, "GPUREG_TEXUNIT0_ADDR2"},
    {0x087, "GPUREG_TEXUNIT0_ADDR3"},
    {0x088, "GPUREG_TEXUNIT0_ADDR4"},
    {0x089, "GPUREG_TEXUNIT0_ADDR5"},
    {0x08A, "GPUREG_TEXUNIT0_ADDR6"},
    {0x08B, "GPUREG_TEXUNIT0_SHADOW"},

    {0x08E, "GPUREG_TEXUNIT0_TYPE"},
    {0x08F, "GPUREG_LIGHTING_ENABLE0"},

    {0x091, "GPUREG_TEXUNIT1_BORDER_COLOR"},
    {0x092, "GPUREG_TEXUNIT1_DIM"},
    {0x093, "GPUREG_TEXUNIT1_PARAM"},
    {0x094, "GPUREG_TEXUNIT1_LOD"},
    {0x095, "GPUREG_TEXUNIT1_ADDR"},
    {0x096, "GPUREG_TEXUNIT1_TYPE"},

    {0x099, "GPUREG_TEXUNIT2_BORDER_COLOR"},
    {0x09A, "GPUREG_TEXUNIT2_DIM"},
    {0x09B, "GPUREG_TEXUNIT2_PARAM"},
    {0x09C, "GPUREG_TEXUNIT2_LOD"},
    {0x09D, "GPUREG_TEXUNIT2_ADDR"},
    {0x09E, "GPUREG_TEXUNIT2_TYPE"},

    {0x0A8, "GPUREG_TEXUNIT3_PROCTEX0"},
    {0x0A9, "GPUREG_TEXUNIT3_PROCTEX1"},
    {0x0AA, "GPUREG_TEXUNIT3_PROCTEX2"},
    {0x0AB, "GPUREG_TEXUNIT3_PROCTEX3"},
    {0x0AC, "GPUREG_TEXUNIT3_PROCTEX4"},
    {0x0AD, "GPUREG_TEXUNIT3_PROCTEX5"},

    {0x0AF, "GPUREG_PROCTEX_LUT"},
    {0x0B0, "GPUREG_PROCTEX_LUT_DATA0"},
    {0x0B1, "GPUREG_PROCTEX_LUT_DATA1"},
    {0x0B2, "GPUREG_PROCTEX_LUT_DATA2"},
    {0x0B3, "GPUREG_PROCTEX_LUT_DATA3"},
    {0x0B4, "GPUREG_PROCTEX_LUT_DATA4"},
    {0x0B5, "GPUREG_PROCTEX_LUT_DATA5"},
    {0x0B6, "GPUREG_PROCTEX_LUT_DATA6"},
    {0x0B7, "GPUREG_PROCTEX_LUT_DATA7"},

    {0x0C0, "GPUREG_TEXENV0_SOURCE"},
    {0x0C1, "GPUREG_TEXENV0_OPERAND"},
    {0x0C2, "GPUREG_TEXENV0_COMBINER"},
    {0x0C3, "GPUREG_TEXENV0_COLOR"},
    {0x0C4, "GPUREG_TEXENV0_SCALE"},

    {0x0C8, "GPUREG_TEXENV1_SOURCE"},
    {0x0C9, "GPUREG_TEXENV1_OPERAND"},
    {0x0CA, "GPUREG_TEXENV1_COMBINER"},
    {0x0CB, "GPUREG_TEXENV1_COLOR"},
    {0x0CC, "GPUREG_TEXENV1_SCALE"},

    {0x0D0, "GPUREG_TEXENV2_SOURCE"},
    {0x0D1, "GPUREG_TEXENV2_OPERAND"},
    {0x0D2, "GPUREG_TEXENV2_COMBINER"},
    {0x0D3, "GPUREG_TEXENV2_COLOR"},
    {0x0D4, "GPUREG_TEXENV2_SCALE"},

    {0x0D8, "GPUREG_TEXENV3_SOURCE"},
    {0x0D9, "GPUREG_TEXENV3_OPERAND"},
    {0x0DA, "GPUREG_TEXENV3_COMBINER"},
    {0x0DB, "GPUREG_TEXENV3_COLOR"},
    {0x0DC, "GPUREG_TEXENV3_SCALE"},

    {0x0E0, "GPUREG_TEXENV_UPDATE_BUFFER"},
    {0x0E1, "GPUREG_FOG_COLOR"},

    {0x0E4, "GPUREG_GAS_ATTENUATION"},
    {0x0E5, "GPUREG_GAS_ACCMAX"},
    {0x0E6, "GPUREG_FOG_LUT_INDEX"},

    {0x0E8, "GPUREG_FOG_LUT_DATA0"},
    {0x0E9, "GPUREG_FOG_LUT_DATA1"},
    {0x0EA, "GPUREG_FOG_LUT_DATA2"},
    {0x0EB, "GPUREG_FOG_LUT_DATA3"},
    {0x0EC, "GPUREG_FOG_LUT_DATA4"},
    {0x0ED, "GPUREG_FOG_LUT_DATA5"},
    {0x0EE, "GPUREG_FOG_LUT_DATA6"},
    {0x0EF, "GPUREG_FOG_LUT_DATA7"},
    {0x0F0, "GPUREG_TEXENV4_SOURCE"},
    {0x0F1, "GPUREG_TEXENV4_OPERAND"},
    {0x0F2, "GPUREG_TEXENV4_COMBINER"},
    {0x0F3, "GPUREG_TEXENV4_COLOR"},
    {0x0F4, "GPUREG_TEXENV4_SCALE"},

    {0x0F8, "GPUREG_TEXENV5_SOURCE"},
    {0x0F9, "GPUREG_TEXENV5_OPERAND"},
    {0x0FA, "GPUREG_TEXENV5_COMBINER"},
    {0x0FB, "GPUREG_TEXENV5_COLOR"},
    {0x0FC, "GPUREG_TEXENV5_SCALE"},
    {0x0FD, "GPUREG_TEXENV_BUFFER_COLOR"},

    {0x100, "GPUREG_COLOR_OPERATION"},
    {0x101, "GPUREG_BLEND_FUNC"},
    {0x102, "GPUREG_LOGIC_OP"},
    {0x103, "GPUREG_BLEND_COLOR"},
    {0x104, "GPUREG_FRAGOP_ALPHA_TEST"},
    {0x105, "GPUREG_STENCIL_TEST"},
    {0x106, "GPUREG_STENCIL_OP"},
    {0x107, "GPUREG_DEPTH_COLOR_MASK"},

    {0x110, "GPUREG_FRAMEBUFFER_INVALIDATE"},
    {0x111, "GPUREG_FRAMEBUFFER_FLUSH"},
    {0x112, "GPUREG_COLORBUFFER_READ"},
    {0x113, "GPUREG_COLORBUFFER_WRITE"},
    {0x114, "GPUREG_DEPTHBUFFER_READ"},
    {0x115, "GPUREG_DEPTHBUFFER_WRITE"},
    {0x116, "GPUREG_DEPTHBUFFER_FORMAT"},
    {0x117, "GPUREG_COLORBUFFER_FORMAT"},
    {0x118, "GPUREG_EARLYDEPTH_TEST2"},

    {0x11B, "GPUREG_FRAMEBUFFER_BLOCK32"},
    {0x11C, "GPUREG_DEPTHBUFFER_LOC"},
    {0x11D, "GPUREG_COLORBUFFER_LOC"},
    {0x11E, "GPUREG_FRAMEBUFFER_DIM"},

    {0x120, "GPUREG_GAS_LIGHT_XY"},
    {0x121, "GPUREG_GAS_LIGHT_Z"},
    {0x122, "GPUREG_GAS_LIGHT_Z_COLOR"},
    {0x123, "GPUREG_GAS_LUT_INDEX"},
    {0x124, "GPUREG_GAS_LUT_DATA"},

    {0x126, "GPUREG_GAS_DELTAZ_DEPTH"},

    {0x130, "GPUREG_FRAGOP_SHADOW"},

    {0x140, "GPUREG_LIGHT0_SPECULAR0"},
    {0x141, "GPUREG_LIGHT0_SPECULAR1"},
    {0x142, "GPUREG_LIGHT0_DIFFUSE"},
    {0x143, "GPUREG_LIGHT0_AMBIENT"},
    {0x144, "GPUREG_LIGHT0_XY"},
    {0x145, "GPUREG_LIGHT0_Z"},
    {0x146, "GPUREG_LIGHT0_SPOTDIR_XY"},
    {0x147, "GPUREG_LIGHT0_SPOTDIR_Z"},

    {0x149, "GPUREG_LIGHT0_CONFIG"},
    {0x14A, "GPUREG_LIGHT0_ATTENUATION_BIAS"},
    {0x14B, "GPUREG_LIGHT0_ATTENUATION_SCALE"},

    {0x150, "GPUREG_LIGHT1_SPECULAR0"},
    {0x151, "GPUREG_LIGHT1_SPECULAR1"},
    {0x152, "GPUREG_LIGHT1_DIFFUSE"},
    {0x153, "GPUREG_LIGHT1_AMBIENT"},
    {0x154, "GPUREG_LIGHT1_XY"},
    {0x155, "GPUREG_LIGHT1_Z"},
    {0x156, "GPUREG_LIGHT1_SPOTDIR_XY"},
    {0x157, "GPUREG_LIGHT1_SPOTDIR_Z"},

    {0x159, "GPUREG_LIGHT1_CONFIG"},
    {0x15A, "GPUREG_LIGHT1_ATTENUATION_BIAS"},
    {0x15B, "GPUREG_LIGHT1_ATTENUATION_SCALE"},

    {0x160, "GPUREG_LIGHT2_SPECULAR0"},
    {0x161, "GPUREG_LIGHT2_SPECULAR1"},
    {0x162, "GPUREG_LIGHT2_DIFFUSE"},
    {0x163, "GPUREG_LIGHT2_AMBIENT"},
    {0x164, "GPUREG_LIGHT2_XY"},
    {0x165, "GPUREG_LIGHT2_Z"},
    {0x166, "GPUREG_LIGHT2_SPOTDIR_XY"},
    {0x167, "GPUREG_LIGHT2_SPOTDIR_Z"},

    {0x169, "GPUREG_LIGHT2_CONFIG"},
    {0x16A, "GPUREG_LIGHT2_ATTENUATION_BIAS"},
    {0x16B, "GPUREG_LIGHT2_ATTENUATION_SCALE"},

    {0x170, "GPUREG_LIGHT3_SPECULAR0"},
    {0x171, "GPUREG_LIGHT3_SPECULAR1"},
    {0x172, "GPUREG_LIGHT3_DIFFUSE"},
    {0x173, "GPUREG_LIGHT3_AMBIENT"},
    {0x174, "GPUREG_LIGHT3_XY"},
    {0x175, "GPUREG_LIGHT3_Z"},
    {0x176, "GPUREG_LIGHT3_SPOTDIR_XY"},
    {0x177, "GPUREG_LIGHT3_SPOTDIR_Z"},

    {0x179, "GPUREG_LIGHT3_CONFIG"},
    {0x17A, "GPUREG_LIGHT3_ATTENUATION_BIAS"},
    {0x17B, "GPUREG_LIGHT3_ATTENUATION_SCALE"},

    {0x180, "GPUREG_LIGHT4_SPECULAR0"},
    {0x181, "GPUREG_LIGHT4_SPECULAR1"},
    {0x182, "GPUREG_LIGHT4_DIFFUSE"},
    {0x183, "GPUREG_LIGHT4_AMBIENT"},
    {0x184, "GPUREG_LIGHT4_XY"},
    {0x185, "GPUREG_LIGHT4_Z"},
    {0x186, "GPUREG_LIGHT4_SPOTDIR_XY"},
    {0x187, "GPUREG_LIGHT4_SPOTDIR_Z"},

    {0x189, "GPUREG_LIGHT4_CONFIG"},
    {0x18A, "GPUREG_LIGHT4_ATTENUATION_BIAS"},
    {0x18B, "GPUREG_LIGHT4_ATTENUATION_SCALE"},

    {0x190, "GPUREG_LIGHT5_SPECULAR0"},
    {0x191, "GPUREG_LIGHT5_SPECULAR1"},
    {0x192, "GPUREG_LIGHT5_DIFFUSE"},
    {0x193, "GPUREG_LIGHT5_AMBIENT"},
    {0x194, "GPUREG_LIGHT5_XY"},
    {0x195, "GPUREG_LIGHT5_Z"},
    {0x196, "GPUREG_LIGHT5_SPOTDIR_XY"},
    {0x197, "GPUREG_LIGHT5_SPOTDIR_Z"},

    {0x199, "GPUREG_LIGHT5_CONFIG"},
    {0x19A, "GPUREG_LIGHT5_ATTENUATION_BIAS"},
    {0x19B, "GPUREG_LIGHT5_ATTENUATION_SCALE"},

    {0x1A0, "GPUREG_LIGHT6_SPECULAR0"},
    {0x1A1, "GPUREG_LIGHT6_SPECULAR1"},
    {0x1A2, "GPUREG_LIGHT6_DIFFUSE"},
    {0x1A3, "GPUREG_LIGHT6_AMBIENT"},
    {0x1A4, "GPUREG_LIGHT6_XY"},
    {0x1A5, "GPUREG_LIGHT6_Z"},
    {0x1A6, "GPUREG_LIGHT6_SPOTDIR_XY"},
    {0x1A7, "GPUREG_LIGHT6_SPOTDIR_Z"},

    {0x1A9, "GPUREG_LIGHT6_CONFIG"},
    {0x1AA, "GPUREG_LIGHT6_ATTENUATION_BIAS"},
    {0x1AB, "GPUREG_LIGHT6_ATTENUATION_SCALE"},

    {0x1B0, "GPUREG_LIGHT7_SPECULAR0"},
    {0x1B1, "GPUREG_LIGHT7_SPECULAR1"},
    {0x1B2, "GPUREG_LIGHT7_DIFFUSE"},
    {0x1B3, "GPUREG_LIGHT7_AMBIENT"},
    {0x1B4, "GPUREG_LIGHT7_XY"},
    {0x1B5, "GPUREG_LIGHT7_Z"},
    {0x1B6, "GPUREG_LIGHT7_SPOTDIR_XY"},
    {0x1B7, "GPUREG_LIGHT7_SPOTDIR_Z"},

    {0x1B9, "GPUREG_LIGHT7_CONFIG"},
    {0x1BA, "GPUREG_LIGHT7_ATTENUATION_BIAS"},
    {0x1BB, "GPUREG_LIGHT7_ATTENUATION_SCALE"},

    {0x1C0, "GPUREG_LIGHTING_AMBIENT"},

    {0x1C2, "GPUREG_LIGHTING_NUM_LIGHTS"},
    {0x1C3, "GPUREG_LIGHTING_CONFIG0"},
    {0x1C4, "GPUREG_LIGHTING_CONFIG1"},
    {0x1C5, "GPUREG_LIGHTING_LUT_INDEX"},
    {0x1C6, "GPUREG_LIGHTING_ENABLE1"},

    {0x1C8, "GPUREG_LIGHTING_LUT_DATA0"},
    {0x1C9, "GPUREG_LIGHTING_LUT_DATA1"},
    {0x1CA, "GPUREG_LIGHTING_LUT_DATA2"},
    {0x1CB, "GPUREG_LIGHTING_LUT_DATA3"},
    {0x1CC, "GPUREG_LIGHTING_LUT_DATA4"},
    {0x1CD, "GPUREG_LIGHTING_LUT_DATA5"},
    {0x1CE, "GPUREG_LIGHTING_LUT_DATA6"},
    {0x1CF, "GPUREG_LIGHTING_LUT_DATA7"},
    {0x1D0, "GPUREG_LIGHTING_LUTINPUT_ABS"},
    {0x1D1, "GPUREG_LIGHTING_LUTINPUT_SELECT"},
    {0x1D2, "GPUREG_LIGHTING_LUTINPUT_SCALE"},

    {0x1D9, "GPUREG_LIGHTING_LIGHT_PERMUTATION"},

    {0x200, "GPUREG_ATTRIBBUFFERS_LOC"},
    {0x201, "GPUREG_ATTRIBBUFFERS_FORMAT_LOW"},
    {0x202, "GPUREG_ATTRIBBUFFERS_FORMAT_HIGH"},
    {0x203, "GPUREG_ATTRIBBUFFER0_OFFSET"},
    {0x204, "GPUREG_ATTRIBBUFFER0_CONFIG1"},
    {0x205, "GPUREG_ATTRIBBUFFER0_CONFIG2"},
    {0x206, "GPUREG_ATTRIBBUFFER1_OFFSET"},
    {0x207, "GPUREG_ATTRIBBUFFER1_CONFIG1"},
    {0x208, "GPUREG_ATTRIBBUFFER1_CONFIG2"},
    {0x209, "GPUREG_ATTRIBBUFFER2_OFFSET"},
    {0x20A, "GPUREG_ATTRIBBUFFER2_CONFIG1"},
    {0x20B, "GPUREG_ATTRIBBUFFER2_CONFIG2"},
    {0x20C, "GPUREG_ATTRIBBUFFER3_OFFSET"},
    {0x20D, "GPUREG_ATTRIBBUFFER3_CONFIG1"},
    {0x20E, "GPUREG_ATTRIBBUFFER3_CONFIG2"},
    {0x20F, "GPUREG_ATTRIBBUFFER4_OFFSET"},
    {0x210, "GPUREG_ATTRIBBUFFER4_CONFIG1"},
    {0x211, "GPUREG_ATTRIBBUFFER4_CONFIG2"},
    {0x212, "GPUREG_ATTRIBBUFFER5_OFFSET"},
    {0x213, "GPUREG_ATTRIBBUFFER5_CONFIG1"},
    {0x214, "GPUREG_ATTRIBBUFFER5_CONFIG2"},
    {0x215, "GPUREG_ATTRIBBUFFER6_OFFSET"},
    {0x216, "GPUREG_ATTRIBBUFFER6_CONFIG1"},
    {0x217, "GPUREG_ATTRIBBUFFER6_CONFIG2"},
    {0x218, "GPUREG_ATTRIBBUFFER7_OFFSET"},
    {0x219, "GPUREG_ATTRIBBUFFER7_CONFIG1"},
    {0x21A, "GPUREG_ATTRIBBUFFER7_CONFIG2"},
    {0x21B, "GPUREG_ATTRIBBUFFER8_OFFSET"},
    {0x21C, "GPUREG_ATTRIBBUFFER8_CONFIG1"},
    {0x21D, "GPUREG_ATTRIBBUFFER8_CONFIG2"},
    {0x21E, "GPUREG_ATTRIBBUFFER9_OFFSET"},
    {0x21F, "GPUREG_ATTRIBBUFFER9_CONFIG1"},
    {0x220, "GPUREG_ATTRIBBUFFER9_CONFIG2"},
    {0x221, "GPUREG_ATTRIBBUFFER10_OFFSET"},
    {0x222, "GPUREG_ATTRIBBUFFER10_CONFIG1"},
    {0x223, "GPUREG_ATTRIBBUFFER10_CONFIG2"},
    {0x224, "GPUREG_ATTRIBBUFFER11_OFFSET"},
    {0x225, "GPUREG_ATTRIBBUFFER11_CONFIG1"},
    {0x226, "GPUREG_ATTRIBBUFFER11_CONFIG2"},
    {0x227, "GPUREG_INDEXBUFFER_CONFIG"},
    {0x228, "GPUREG_NUMVERTICES"},
    {0x229, "GPUREG_GEOSTAGE_CONFIG"},
    {0x22A, "GPUREG_VERTEX_OFFSET"},

    {0x22D, "GPUREG_POST_VERTEX_CACHE_NUM"},
    {0x22E, "GPUREG_DRAWARRAYS"},
    {0x22F, "GPUREG_DRAWELEMENTS"},

    {0x231, "GPUREG_VTX_FUNC"},
    {0x232, "GPUREG_FIXEDATTRIB_INDEX"},
    {0x233, "GPUREG_FIXEDATTRIB_DATA0"},
    {0x234, "GPUREG_FIXEDATTRIB_DATA1"},
    {0x235, "GPUREG_FIXEDATTRIB_DATA2"},

    {0x238, "GPUREG_CMDBUF_SIZE0"},
    {0x239, "GPUREG_CMDBUF_SIZE1"},
    {0x23A, "GPUREG_CMDBUF_ADDR0"},
    {0x23B, "GPUREG_CMDBUF_ADDR1"},
    {0x23C, "GPUREG_CMDBUF_JUMP0"},
    {0x23D, "GPUREG_CMDBUF_JUMP1"},

    {0x242, "GPUREG_VSH_NUM_ATTR"},

    {0x244, "GPUREG_VSH_COM_MODE"},
    {0x245, "GPUREG_START_DRAW_FUNC0"},

    {0x24A, "GPUREG_VSH_OUTMAP_TOTAL1"},

    {0x251, "GPUREG_VSH_OUTMAP_TOTAL2"},
    {0x252, "GPUREG_GSH_MISC0"},
    {0x253, "GPUREG_GEOSTAGE_CONFIG2"},
    {0x254, "GPUREG_GSH_MISC1"},

    {0x25E, "GPUREG_PRIMITIVE_CONFIG"},
    {0x25F, "GPUREG_RESTART_PRIMITIVE"},

    {0x280, "GPUREG_GSH_BOOLUNIFORM"},
    {0x281, "GPUREG_GSH_INTUNIFORM_I0"},
    {0x282, "GPUREG_GSH_INTUNIFORM_I1"},
    {0x283, "GPUREG_GSH_INTUNIFORM_I2"},
    {0x284, "GPUREG_GSH_INTUNIFORM_I3"},

    {0x289, "GPUREG_GSH_INPUTBUFFER_CONFIG"},
    {0x28A, "GPUREG_GSH_ENTRYPOINT"},
    {0x28B, "GPUREG_GSH_ATTRIBUTES_PERMUTATION_LOW"},
    {0x28C, "GPUREG_GSH_ATTRIBUTES_PERMUTATION_HIGH"},
    {0x28D, "GPUREG_GSH_OUTMAP_MASK"},

    {0x28F, "GPUREG_GSH_CODETRANSFER_END"},
    {0x290, "GPUREG_GSH_FLOATUNIFORM_INDEX"},
    {0x291, "GPUREG_GSH_FLOATUNIFORM_DATA0"},
    {0x292, "GPUREG_GSH_FLOATUNIFORM_DATA1"},
    {0x293, "GPUREG_GSH_FLOATUNIFORM_DATA2"},
    {0x294, "GPUREG_GSH_FLOATUNIFORM_DATA3"},
    {0x295, "GPUREG_GSH_FLOATUNIFORM_DATA4"},
    {0x296, "GPUREG_GSH_FLOATUNIFORM_DATA5"},
    {0x297, "GPUREG_GSH_FLOATUNIFORM_DATA6"},
    {0x298, "GPUREG_GSH_FLOATUNIFORM_DATA7"},

    {0x29B, "GPUREG_GSH_CODETRANSFER_INDEX"},
    {0x29C, "GPUREG_GSH_CODETRANSFER_DATA0"},
    {0x29D, "GPUREG_GSH_CODETRANSFER_DATA1"},
    {0x29E, "GPUREG_GSH_CODETRANSFER_DATA2"},
    {0x29F, "GPUREG_GSH_CODETRANSFER_DATA3"},
    {0x2A0, "GPUREG_GSH_CODETRANSFER_DATA4"},
    {0x2A1, "GPUREG_GSH_CODETRANSFER_DATA5"},
    {0x2A2, "GPUREG_GSH_CODETRANSFER_DATA6"},
    {0x2A3, "GPUREG_GSH_CODETRANSFER_DATA7"},

    {0x2A5, "GPUREG_GSH_OPDESCS_INDEX"},
    {0x2A6, "GPUREG_GSH_OPDESCS_DATA0"},
    {0x2A7, "GPUREG_GSH_OPDESCS_DATA1"},
    {0x2A8, "GPUREG_GSH_OPDESCS_DATA2"},
    {0x2A9, "GPUREG_GSH_OPDESCS_DATA3"},
    {0x2AA, "GPUREG_GSH_OPDESCS_DATA4"},
    {0x2AB, "GPUREG_GSH_OPDESCS_DATA5"},
    {0x2AC, "GPUREG_GSH_OPDESCS_DATA6"},
    {0x2AD, "GPUREG_GSH_OPDESCS_DATA7"},

    {0x2B0, "GPUREG_VSH_BOOLUNIFORM"},
    {0x2B1, "GPUREG_VSH_INTUNIFORM_I0"},
    {0x2B2, "GPUREG_VSH_INTUNIFORM_I1"},
    {0x2B3, "GPUREG_VSH_INTUNIFORM_I2"},
    {0x2B4, "GPUREG_VSH_INTUNIFORM_I3"},

    {0x2B9, "GPUREG_VSH_INPUTBUFFER_CONFIG"},
    {0x2BA, "GPUREG_VSH_ENTRYPOINT"},
    {0x2BB, "GPUREG_VSH_ATTRIBUTES_PERMUTATION_LOW"},
    {0x2BC, "GPUREG_VSH_ATTRIBUTES_PERMUTATION_HIGH"},
    {0x2BD, "GPUREG_VSH_OUTMAP_MASK"},

    {0x2BF, "GPUREG_VSH_CODETRANSFER_END"},
    {0x2C0, "GPUREG_VSH_FLOATUNIFORM_INDEX"},
    {0x2C1, "GPUREG_VSH_FLOATUNIFORM_DATA0"},
    {0x2C2, "GPUREG_VSH_FLOATUNIFORM_DATA1"},
    {0x2C3, "GPUREG_VSH_FLOATUNIFORM_DATA2"},
    {0x2C4, "GPUREG_VSH_FLOATUNIFORM_DATA3"},
    {0x2C5, "GPUREG_VSH_FLOATUNIFORM_DATA4"},
    {0x2C6, "GPUREG_VSH_FLOATUNIFORM_DATA5"},
    {0x2C7, "GPUREG_VSH_FLOATUNIFORM_DATA6"},
    {0x2C8, "GPUREG_VSH_FLOATUNIFORM_DATA7"},

    {0x2CB, "GPUREG_VSH_CODETRANSFER_INDEX"},
    {0x2CC, "GPUREG_VSH_CODETRANSFER_DATA0"},
    {0x2CD, "GPUREG_VSH_CODETRANSFER_DATA1"},
    {0x2CE, "GPUREG_VSH_CODETRANSFER_DATA2"},
    {0x2CF, "GPUREG_VSH_CODETRANSFER_DATA3"},
    {0x2D0, "GPUREG_VSH_CODETRANSFER_DATA4"},
    {0x2D1, "GPUREG_VSH_CODETRANSFER_DATA5"},
    {0x2D2, "GPUREG_VSH_CODETRANSFER_DATA6"},
    {0x2D3, "GPUREG_VSH_CODETRANSFER_DATA7"},

    {0x2D5, "GPUREG_VSH_OPDESCS_INDEX"},
    {0x2D6, "GPUREG_VSH_OPDESCS_DATA0"},
    {0x2D7, "GPUREG_VSH_OPDESCS_DATA1"},
    {0x2D8, "GPUREG_VSH_OPDESCS_DATA2"},
    {0x2D9, "GPUREG_VSH_OPDESCS_DATA3"},
    {0x2DA, "GPUREG_VSH_OPDESCS_DATA4"},
    {0x2DB, "GPUREG_VSH_OPDESCS_DATA5"},
    {0x2DC, "GPUREG_VSH_OPDESCS_DATA6"},
    {0x2DD, "GPUREG_VSH_OPDESCS_DATA7"},
};

const char* RegsInternal::GetRegisterName(u16 index) {
    const auto it = std::lower_bound(std::begin(register_names), std::end(register_names), index,
                                     [](auto p, auto i) { return p.first < i; });
    if (it->first == index) {
        return it->second;
    }

    // Return empty string if no match is found
    return "";
}

} // namespace Pica
