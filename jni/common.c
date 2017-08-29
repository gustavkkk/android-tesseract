#include "common.h"


// RGB -> YUV
#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)


//#define GET_B(CLR)	(((CLR & 0x1F) * 8) >> 5)
//#define GET_G(CLR)	((((CLR >> 5) & 0x3F) * 255) >> 6)
//#define GET_R(CLR)	((((CLR >> 11) & 0x1F) * 255) >> 5)
#define GET_B(CLR)	((CLR & 0x1F) << 3)
#define GET_G(CLR)	(((CLR >> 5) & 0x3F) << 2)
#define GET_R(CLR)	(((CLR >> 11) & 0x1F) << 3)

// lookup tables
int lookup_m_94[] = {
	0,    -94,   -188,   -282,   -376,   -470,   -564,   -658,
	-752,   -846,   -940,  -1034,  -1128,  -1222,  -1316,  -1410,
	-1504,  -1598,  -1692,  -1786,  -1880,  -1974,  -2068,  -2162,
	-2256,  -2350,  -2444,  -2538,  -2632,  -2726,  -2820,  -2914,
	-3008,  -3102,  -3196,  -3290,  -3384,  -3478,  -3572,  -3666,
	-3760,  -3854,  -3948,  -4042,  -4136,  -4230,  -4324,  -4418,
	-4512,  -4606,  -4700,  -4794,  -4888,  -4982,  -5076,  -5170,
	-5264,  -5358,  -5452,  -5546,  -5640,  -5734,  -5828,  -5922,
	-6016,  -6110,  -6204,  -6298,  -6392,  -6486,  -6580,  -6674,
	-6768,  -6862,  -6956,  -7050,  -7144,  -7238,  -7332,  -7426,
	-7520,  -7614,  -7708,  -7802,  -7896,  -7990,  -8084,  -8178,
	-8272,  -8366,  -8460,  -8554,  -8648,  -8742,  -8836,  -8930,
	-9024,  -9118,  -9212,  -9306,  -9400,  -9494,  -9588,  -9682,
	-9776,  -9870,  -9964, -10058, -10152, -10246, -10340, -10434,
	-10528, -10622, -10716, -10810, -10904, -10998, -11092, -11186,
	-11280, -11374, -11468, -11562, -11656, -11750, -11844, -11938,
	-12032, -12126, -12220, -12314, -12408, -12502, -12596, -12690,
	-12784, -12878, -12972, -13066, -13160, -13254, -13348, -13442,
	-13536, -13630, -13724, -13818, -13912, -14006, -14100, -14194,
	-14288, -14382, -14476, -14570, -14664, -14758, -14852, -14946,
	-15040, -15134, -15228, -15322, -15416, -15510, -15604, -15698,
	-15792, -15886, -15980, -16074, -16168, -16262, -16356, -16450,
	-16544, -16638, -16732, -16826, -16920, -17014, -17108, -17202,
	-17296, -17390, -17484, -17578, -17672, -17766, -17860, -17954,
	-18048, -18142, -18236, -18330, -18424, -18518, -18612, -18706,
	-18800, -18894, -18988, -19082, -19176, -19270, -19364, -19458,
	-19552, -19646, -19740, -19834, -19928, -20022, -20116, -20210,
	-20304, -20398, -20492, -20586, -20680, -20774, -20868, -20962,
	-21056, -21150, -21244, -21338, -21432, -21526, -21620, -21714,
	-21808, -21902, -21996, -22090, -22184, -22278, -22372, -22466,
	-22560, -22654, -22748, -22842, -22936, -23030, -23124, -23218,
	-23312, -23406, -23500, -23594, -23688, -23782, -23876, -23970
};

int lookup_m_74[] = {
	0,    -74,   -148,   -222,   -296,   -370,   -444,   -518,
	-592,   -666,   -740,   -814,   -888,   -962,  -1036,  -1110,
	-1184,  -1258,  -1332,  -1406,  -1480,  -1554,  -1628,  -1702,
	-1776,  -1850,  -1924,  -1998,  -2072,  -2146,  -2220,  -2294,
	-2368,  -2442,  -2516,  -2590,  -2664,  -2738,  -2812,  -2886,
	-2960,  -3034,  -3108,  -3182,  -3256,  -3330,  -3404,  -3478,
	-3552,  -3626,  -3700,  -3774,  -3848,  -3922,  -3996,  -4070,
	-4144,  -4218,  -4292,  -4366,  -4440,  -4514,  -4588,  -4662,
	-4736,  -4810,  -4884,  -4958,  -5032,  -5106,  -5180,  -5254,
	-5328,  -5402,  -5476,  -5550,  -5624,  -5698,  -5772,  -5846,
	-5920,  -5994,  -6068,  -6142,  -6216,  -6290,  -6364,  -6438,
	-6512,  -6586,  -6660,  -6734,  -6808,  -6882,  -6956,  -7030,
	-7104,  -7178,  -7252,  -7326,  -7400,  -7474,  -7548,  -7622,
	-7696,  -7770,  -7844,  -7918,  -7992,  -8066,  -8140,  -8214,
	-8288,  -8362,  -8436,  -8510,  -8584,  -8658,  -8732,  -8806,
	-8880,  -8954,  -9028,  -9102,  -9176,  -9250,  -9324,  -9398,
	-9472,  -9546,  -9620,  -9694,  -9768,  -9842,  -9916,  -9990,
	-10064, -10138, -10212, -10286, -10360, -10434, -10508, -10582,
	-10656, -10730, -10804, -10878, -10952, -11026, -11100, -11174,
	-11248, -11322, -11396, -11470, -11544, -11618, -11692, -11766,
	-11840, -11914, -11988, -12062, -12136, -12210, -12284, -12358,
	-12432, -12506, -12580, -12654, -12728, -12802, -12876, -12950,
	-13024, -13098, -13172, -13246, -13320, -13394, -13468, -13542,
	-13616, -13690, -13764, -13838, -13912, -13986, -14060, -14134,
	-14208, -14282, -14356, -14430, -14504, -14578, -14652, -14726,
	-14800, -14874, -14948, -15022, -15096, -15170, -15244, -15318,
	-15392, -15466, -15540, -15614, -15688, -15762, -15836, -15910,
	-15984, -16058, -16132, -16206, -16280, -16354, -16428, -16502,
	-16576, -16650, -16724, -16798, -16872, -16946, -17020, -17094,
	-17168, -17242, -17316, -17390, -17464, -17538, -17612, -17686,
	-17760, -17834, -17908, -17982, -18056, -18130, -18204, -18278,
	-18352, -18426, -18500, -18574, -18648, -18722, -18796, -18870
};

int lookup_m_38[] = {
	0,    -38,    -76,   -114,   -152,   -190,   -228,   -266,
	-304,   -342,   -380,   -418,   -456,   -494,   -532,   -570,
	-608,   -646,   -684,   -722,   -760,   -798,   -836,   -874,
	-912,   -950,   -988,  -1026,  -1064,  -1102,  -1140,  -1178,
	-1216,  -1254,  -1292,  -1330,  -1368,  -1406,  -1444,  -1482,
	-1520,  -1558,  -1596,  -1634,  -1672,  -1710,  -1748,  -1786,
	-1824,  -1862,  -1900,  -1938,  -1976,  -2014,  -2052,  -2090,
	-2128,  -2166,  -2204,  -2242,  -2280,  -2318,  -2356,  -2394,
	-2432,  -2470,  -2508,  -2546,  -2584,  -2622,  -2660,  -2698,
	-2736,  -2774,  -2812,  -2850,  -2888,  -2926,  -2964,  -3002,
	-3040,  -3078,  -3116,  -3154,  -3192,  -3230,  -3268,  -3306,
	-3344,  -3382,  -3420,  -3458,  -3496,  -3534,  -3572,  -3610,
	-3648,  -3686,  -3724,  -3762,  -3800,  -3838,  -3876,  -3914,
	-3952,  -3990,  -4028,  -4066,  -4104,  -4142,  -4180,  -4218,
	-4256,  -4294,  -4332,  -4370,  -4408,  -4446,  -4484,  -4522,
	-4560,  -4598,  -4636,  -4674,  -4712,  -4750,  -4788,  -4826,
	-4864,  -4902,  -4940,  -4978,  -5016,  -5054,  -5092,  -5130,
	-5168,  -5206,  -5244,  -5282,  -5320,  -5358,  -5396,  -5434,
	-5472,  -5510,  -5548,  -5586,  -5624,  -5662,  -5700,  -5738,
	-5776,  -5814,  -5852,  -5890,  -5928,  -5966,  -6004,  -6042,
	-6080,  -6118,  -6156,  -6194,  -6232,  -6270,  -6308,  -6346,
	-6384,  -6422,  -6460,  -6498,  -6536,  -6574,  -6612,  -6650,
	-6688,  -6726,  -6764,  -6802,  -6840,  -6878,  -6916,  -6954,
	-6992,  -7030,  -7068,  -7106,  -7144,  -7182,  -7220,  -7258,
	-7296,  -7334,  -7372,  -7410,  -7448,  -7486,  -7524,  -7562,
	-7600,  -7638,  -7676,  -7714,  -7752,  -7790,  -7828,  -7866,
	-7904,  -7942,  -7980,  -8018,  -8056,  -8094,  -8132,  -8170,
	-8208,  -8246,  -8284,  -8322,  -8360,  -8398,  -8436,  -8474,
	-8512,  -8550,  -8588,  -8626,  -8664,  -8702,  -8740,  -8778,
	-8816,  -8854,  -8892,  -8930,  -8968,  -9006,  -9044,  -9082,
	-9120,  -9158,  -9196,  -9234,  -9272,  -9310,  -9348,  -9386,
	-9424,  -9462,  -9500,  -9538,  -9576,  -9614,  -9652,  -9690
};

int lookup_m_18[] = {
	0,    -18,    -36,    -54,    -72,    -90,   -108,   -126,
	-144,   -162,   -180,   -198,   -216,   -234,   -252,   -270,
	-288,   -306,   -324,   -342,   -360,   -378,   -396,   -414,
	-432,   -450,   -468,   -486,   -504,   -522,   -540,   -558,
	-576,   -594,   -612,   -630,   -648,   -666,   -684,   -702,
	-720,   -738,   -756,   -774,   -792,   -810,   -828,   -846,
	-864,   -882,   -900,   -918,   -936,   -954,   -972,   -990,
	-1008,  -1026,  -1044,  -1062,  -1080,  -1098,  -1116,  -1134,
	-1152,  -1170,  -1188,  -1206,  -1224,  -1242,  -1260,  -1278,
	-1296,  -1314,  -1332,  -1350,  -1368,  -1386,  -1404,  -1422,
	-1440,  -1458,  -1476,  -1494,  -1512,  -1530,  -1548,  -1566,
	-1584,  -1602,  -1620,  -1638,  -1656,  -1674,  -1692,  -1710,
	-1728,  -1746,  -1764,  -1782,  -1800,  -1818,  -1836,  -1854,
	-1872,  -1890,  -1908,  -1926,  -1944,  -1962,  -1980,  -1998,
	-2016,  -2034,  -2052,  -2070,  -2088,  -2106,  -2124,  -2142,
	-2160,  -2178,  -2196,  -2214,  -2232,  -2250,  -2268,  -2286,
	-2304,  -2322,  -2340,  -2358,  -2376,  -2394,  -2412,  -2430,
	-2448,  -2466,  -2484,  -2502,  -2520,  -2538,  -2556,  -2574,
	-2592,  -2610,  -2628,  -2646,  -2664,  -2682,  -2700,  -2718,
	-2736,  -2754,  -2772,  -2790,  -2808,  -2826,  -2844,  -2862,
	-2880,  -2898,  -2916,  -2934,  -2952,  -2970,  -2988,  -3006,
	-3024,  -3042,  -3060,  -3078,  -3096,  -3114,  -3132,  -3150,
	-3168,  -3186,  -3204,  -3222,  -3240,  -3258,  -3276,  -3294,
	-3312,  -3330,  -3348,  -3366,  -3384,  -3402,  -3420,  -3438,
	-3456,  -3474,  -3492,  -3510,  -3528,  -3546,  -3564,  -3582,
	-3600,  -3618,  -3636,  -3654,  -3672,  -3690,  -3708,  -3726,
	-3744,  -3762,  -3780,  -3798,  -3816,  -3834,  -3852,  -3870,
	-3888,  -3906,  -3924,  -3942,  -3960,  -3978,  -3996,  -4014,
	-4032,  -4050,  -4068,  -4086,  -4104,  -4122,  -4140,  -4158,
	-4176,  -4194,  -4212,  -4230,  -4248,  -4266,  -4284,  -4302,
	-4320,  -4338,  -4356,  -4374,  -4392,  -4410,  -4428,  -4446,
	-4464,  -4482,  -4500,  -4518,  -4536,  -4554,  -4572,  -4590
};

int lookup25[] = {
	0,     25,     50,     75,    100,    125,    150,    175,
	200,    225,    250,    275,    300,    325,    350,    375,
	400,    425,    450,    475,    500,    525,    550,    575,
	600,    625,    650,    675,    700,    725,    750,    775,
	800,    825,    850,    875,    900,    925,    950,    975,
	1000,   1025,   1050,   1075,   1100,   1125,   1150,   1175,
	1200,   1225,   1250,   1275,   1300,   1325,   1350,   1375,
	1400,   1425,   1450,   1475,   1500,   1525,   1550,   1575,
	1600,   1625,   1650,   1675,   1700,   1725,   1750,   1775,
	1800,   1825,   1850,   1875,   1900,   1925,   1950,   1975,
	2000,   2025,   2050,   2075,   2100,   2125,   2150,   2175,
	2200,   2225,   2250,   2275,   2300,   2325,   2350,   2375,
	2400,   2425,   2450,   2475,   2500,   2525,   2550,   2575,
	2600,   2625,   2650,   2675,   2700,   2725,   2750,   2775,
	2800,   2825,   2850,   2875,   2900,   2925,   2950,   2975,
	3000,   3025,   3050,   3075,   3100,   3125,   3150,   3175,
	3200,   3225,   3250,   3275,   3300,   3325,   3350,   3375,
	3400,   3425,   3450,   3475,   3500,   3525,   3550,   3575,
	3600,   3625,   3650,   3675,   3700,   3725,   3750,   3775,
	3800,   3825,   3850,   3875,   3900,   3925,   3950,   3975,
	4000,   4025,   4050,   4075,   4100,   4125,   4150,   4175,
	4200,   4225,   4250,   4275,   4300,   4325,   4350,   4375,
	4400,   4425,   4450,   4475,   4500,   4525,   4550,   4575,
	4600,   4625,   4650,   4675,   4700,   4725,   4750,   4775,
	4800,   4825,   4850,   4875,   4900,   4925,   4950,   4975,
	5000,   5025,   5050,   5075,   5100,   5125,   5150,   5175,
	5200,   5225,   5250,   5275,   5300,   5325,   5350,   5375,
	5400,   5425,   5450,   5475,   5500,   5525,   5550,   5575,
	5600,   5625,   5650,   5675,   5700,   5725,   5750,   5775,
	5800,   5825,   5850,   5875,   5900,   5925,   5950,   5975,
	6000,   6025,   6050,   6075,   6100,   6125,   6150,   6175,
	6200,   6225,   6250,   6275,   6300,   6325,   6350,   6375
};

int lookup66[] = {
	0,     66,    132,    198,    264,    330,    396,    462,
	528,    594,    660,    726,    792,    858,    924,    990,
	1056,   1122,   1188,   1254,   1320,   1386,   1452,   1518,
	1584,   1650,   1716,   1782,   1848,   1914,   1980,   2046,
	2112,   2178,   2244,   2310,   2376,   2442,   2508,   2574,
	2640,   2706,   2772,   2838,   2904,   2970,   3036,   3102,
	3168,   3234,   3300,   3366,   3432,   3498,   3564,   3630,
	3696,   3762,   3828,   3894,   3960,   4026,   4092,   4158,
	4224,   4290,   4356,   4422,   4488,   4554,   4620,   4686,
	4752,   4818,   4884,   4950,   5016,   5082,   5148,   5214,
	5280,   5346,   5412,   5478,   5544,   5610,   5676,   5742,
	5808,   5874,   5940,   6006,   6072,   6138,   6204,   6270,
	6336,   6402,   6468,   6534,   6600,   6666,   6732,   6798,
	6864,   6930,   6996,   7062,   7128,   7194,   7260,   7326,
	7392,   7458,   7524,   7590,   7656,   7722,   7788,   7854,
	7920,   7986,   8052,   8118,   8184,   8250,   8316,   8382,
	8448,   8514,   8580,   8646,   8712,   8778,   8844,   8910,
	8976,   9042,   9108,   9174,   9240,   9306,   9372,   9438,
	9504,   9570,   9636,   9702,   9768,   9834,   9900,   9966,
	10032,  10098,  10164,  10230,  10296,  10362,  10428,  10494,
	10560,  10626,  10692,  10758,  10824,  10890,  10956,  11022,
	11088,  11154,  11220,  11286,  11352,  11418,  11484,  11550,
	11616,  11682,  11748,  11814,  11880,  11946,  12012,  12078,
	12144,  12210,  12276,  12342,  12408,  12474,  12540,  12606,
	12672,  12738,  12804,  12870,  12936,  13002,  13068,  13134,
	13200,  13266,  13332,  13398,  13464,  13530,  13596,  13662,
	13728,  13794,  13860,  13926,  13992,  14058,  14124,  14190,
	14256,  14322,  14388,  14454,  14520,  14586,  14652,  14718,
	14784,  14850,  14916,  14982,  15048,  15114,  15180,  15246,
	15312,  15378,  15444,  15510,  15576,  15642,  15708,  15774,
	15840,  15906,  15972,  16038,  16104,  16170,  16236,  16302,
	16368,  16434,  16500,  16566,  16632,  16698,  16764,  16830
};

int lookup112[] = {
	0,    112,    224,    336,    448,    560,    672,    784,
	896,   1008,   1120,   1232,   1344,   1456,   1568,   1680,
	1792,   1904,   2016,   2128,   2240,   2352,   2464,   2576,
	2688,   2800,   2912,   3024,   3136,   3248,   3360,   3472,
	3584,   3696,   3808,   3920,   4032,   4144,   4256,   4368,
	4480,   4592,   4704,   4816,   4928,   5040,   5152,   5264,
	5376,   5488,   5600,   5712,   5824,   5936,   6048,   6160,
	6272,   6384,   6496,   6608,   6720,   6832,   6944,   7056,
	7168,   7280,   7392,   7504,   7616,   7728,   7840,   7952,
	8064,   8176,   8288,   8400,   8512,   8624,   8736,   8848,
	8960,   9072,   9184,   9296,   9408,   9520,   9632,   9744,
	9856,   9968,  10080,  10192,  10304,  10416,  10528,  10640,
	10752,  10864,  10976,  11088,  11200,  11312,  11424,  11536,
	11648,  11760,  11872,  11984,  12096,  12208,  12320,  12432,
	12544,  12656,  12768,  12880,  12992,  13104,  13216,  13328,
	13440,  13552,  13664,  13776,  13888,  14000,  14112,  14224,
	14336,  14448,  14560,  14672,  14784,  14896,  15008,  15120,
	15232,  15344,  15456,  15568,  15680,  15792,  15904,  16016,
	16128,  16240,  16352,  16464,  16576,  16688,  16800,  16912,
	17024,  17136,  17248,  17360,  17472,  17584,  17696,  17808,
	17920,  18032,  18144,  18256,  18368,  18480,  18592,  18704,
	18816,  18928,  19040,  19152,  19264,  19376,  19488,  19600,
	19712,  19824,  19936,  20048,  20160,  20272,  20384,  20496,
	20608,  20720,  20832,  20944,  21056,  21168,  21280,  21392,
	21504,  21616,  21728,  21840,  21952,  22064,  22176,  22288,
	22400,  22512,  22624,  22736,  22848,  22960,  23072,  23184,
	23296,  23408,  23520,  23632,  23744,  23856,  23968,  24080,
	24192,  24304,  24416,  24528,  24640,  24752,  24864,  24976,
	25088,  25200,  25312,  25424,  25536,  25648,  25760,  25872,
	25984,  26096,  26208,  26320,  26432,  26544,  26656,  26768,
	26880,  26992,  27104,  27216,  27328,  27440,  27552,  27664,
	27776,  27888,  28000,  28112,  28224,  28336,  28448,  28560
};

int lookup129[] = {
	0,    129,    258,    387,    516,    645,    774,    903,
	1032,   1161,   1290,   1419,   1548,   1677,   1806,   1935,
	2064,   2193,   2322,   2451,   2580,   2709,   2838,   2967,
	3096,   3225,   3354,   3483,   3612,   3741,   3870,   3999,
	4128,   4257,   4386,   4515,   4644,   4773,   4902,   5031,
	5160,   5289,   5418,   5547,   5676,   5805,   5934,   6063,
	6192,   6321,   6450,   6579,   6708,   6837,   6966,   7095,
	7224,   7353,   7482,   7611,   7740,   7869,   7998,   8127,
	8256,   8385,   8514,   8643,   8772,   8901,   9030,   9159,
	9288,   9417,   9546,   9675,   9804,   9933,  10062,  10191,
	10320,  10449,  10578,  10707,  10836,  10965,  11094,  11223,
	11352,  11481,  11610,  11739,  11868,  11997,  12126,  12255,
	12384,  12513,  12642,  12771,  12900,  13029,  13158,  13287,
	13416,  13545,  13674,  13803,  13932,  14061,  14190,  14319,
	14448,  14577,  14706,  14835,  14964,  15093,  15222,  15351,
	15480,  15609,  15738,  15867,  15996,  16125,  16254,  16383,
	16512,  16641,  16770,  16899,  17028,  17157,  17286,  17415,
	17544,  17673,  17802,  17931,  18060,  18189,  18318,  18447,
	18576,  18705,  18834,  18963,  19092,  19221,  19350,  19479,
	19608,  19737,  19866,  19995,  20124,  20253,  20382,  20511,
	20640,  20769,  20898,  21027,  21156,  21285,  21414,  21543,
	21672,  21801,  21930,  22059,  22188,  22317,  22446,  22575,
	22704,  22833,  22962,  23091,  23220,  23349,  23478,  23607,
	23736,  23865,  23994,  24123,  24252,  24381,  24510,  24639,
	24768,  24897,  25026,  25155,  25284,  25413,  25542,  25671,
	25800,  25929,  26058,  26187,  26316,  26445,  26574,  26703,
	26832,  26961,  27090,  27219,  27348,  27477,  27606,  27735,
	27864,  27993,  28122,  28251,  28380,  28509,  28638,  28767,
	28896,  29025,  29154,  29283,  29412,  29541,  29670,  29799,
	29928,  30057,  30186,  30315,  30444,  30573,  30702,  30831,
	30960,  31089,  31218,  31347,  31476,  31605,  31734,  31863,
	31992,  32121,  32250,  32379,  32508,  32637,  32766,  32895
};


int bgr565_to_yuv420sp(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height)
{
	int idx;
	int idy;

	int size1 = width * height;

	short* pRGB = (short*)a_pSrcBuffRGB;
	char* pY = a_pDstBuffYUV;
	char* pUV = a_pDstBuffYUV + size1;

	int idxUV = 0;
	int odd = 0;

	pRGB += (size1 - width);

	for (idy=0; idy<height; idy++){
		for(idx=0; idx<width; idx+=2){
			short clr1 = (*pRGB);
			pRGB ++;

			unsigned char b1 = GET_R(clr1);
			unsigned char g1 = GET_G(clr1);
			unsigned char r1 = GET_B(clr1);

			short clr2 = (*pRGB);
			pRGB ++;

			unsigned char b2 = GET_R(clr2);
			unsigned char g2 = GET_G(clr2);
			unsigned char r2 = GET_B(clr2);

			(*pY) = ((lookup66[r1] + lookup129[g1] + lookup25[b1]) >> 8) + 16;
			pY ++;

			(*pY) = ((lookup66[r2] + lookup129[g2] + lookup25[b2]) >> 8) + 16;
			pY ++;

			if (odd == 0)
			{
				(*pUV) = ((lookup_m_38[r1] + lookup_m_74[g1] + lookup112[b1]) >> 8) + 128;
				pUV ++;

				(*pUV) = ((lookup112[r1] + lookup_m_94[g1] + lookup_m_18[b1]) >> 8) + 128;
				pUV ++;
			}
		}

		odd = (!odd);
		pRGB -= (width<<1);
	}

	return 1;
}

// convert RGB565 to yuv420sp.
int rgb565_to_yuv420sp(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height)
{
	int idx;
	int idy;

	int size1 = width * height;

	short* pRGB = (short*)a_pSrcBuffRGB;
	char* pY = a_pDstBuffYUV;
	char* pUV = a_pDstBuffYUV + size1;

	int idxUV = 0;
	int odd = 1;

	pRGB += (size1 - width);

	for (idy=0; idy<height; idy++){
		for(idx=0; idx<width; idx+=2){
			short clr1 = (*pRGB);
			pRGB ++;

			unsigned char r1 = GET_R(clr1);
			unsigned char g1 = GET_G(clr1);
			unsigned char b1 = GET_B(clr1);

			short clr2 = (*pRGB);
			pRGB ++;

			unsigned char r2 = GET_R(clr2);
			unsigned char g2 = GET_G(clr2);
			unsigned char b2 = GET_B(clr2);

			(*pY) = ((lookup66[r1] + lookup129[g1] + lookup25[b1]) >> 8) + 16;
			pY ++;

			(*pY) = ((lookup66[r2] + lookup129[g2] + lookup25[b2]) >> 8) + 16;
			pY ++;

			if (odd & 0x01)
			{
				(*pUV) = ((lookup_m_38[r1] + lookup_m_74[g1] + lookup112[b1]) >> 8) + 128;
				pUV ++;

				(*pUV) = ((lookup112[r1] + lookup_m_94[g1] + lookup_m_18[b1]) >> 8) + 128;
				pUV ++;
			}
		}

		odd = (!odd);
		pRGB -= (width<<1);
	}

	return 1;
}

int bgr8888_to_yuv420sp(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height)
{
	int idx;
	int idy;

	int size1 = width * height;

	unsigned char* pRGB = (unsigned char*)a_pSrcBuffRGB;
	char* pY = a_pDstBuffYUV;
	char* pUV = a_pDstBuffYUV + size1;

	int odd = 1;

	pRGB += (size1 - width) << 2; // * 4

	for (idy=0; idy<height; idy++){
		for(idx=0; idx<width; idx+=2){
			unsigned char b1 = (*pRGB);
			pRGB ++;
			unsigned char g1 = (*pRGB);
			pRGB ++;
			unsigned char r1 = (*pRGB);
			pRGB ++;
			unsigned char a1 = (*pRGB);
			pRGB ++;

			unsigned char b2 = (*pRGB);
			pRGB ++;
			unsigned char g2 = (*pRGB);
			pRGB ++;
			unsigned char r2 = (*pRGB);
			pRGB ++;
			unsigned char a2 = (*pRGB);
			pRGB ++;

			(*pY) = ((lookup66[r1] + lookup129[g1] + lookup25[b1]) >> 8) + 16;
			pY ++;

			(*pY) = ((lookup66[r2] + lookup129[g2] + lookup25[b2]) >> 8) + 16;
			pY ++;

			if (odd & 0x01)
			{
				(*pUV) = ((lookup_m_38[r1] + lookup_m_74[g1] + lookup112[b1]) >> 8) + 128;
				pUV ++;

				(*pUV) = ((lookup112[r1] + lookup_m_94[g1] + lookup_m_18[b1]) >> 8) + 128;
				pUV ++;
			}
		}

		odd = (!odd);
		pRGB -= (width<<3); // * 8
	}

	return 1;
}

// convert RGB8888 to yuv420sp.
int rgb8888_to_yuv420sp(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height)
{
	int idx;
	int idy;

	int size1 = width * height;

	unsigned char* pRGB = (unsigned char*)a_pSrcBuffRGB;
	char* pY = a_pDstBuffYUV;
	char* pUV = a_pDstBuffYUV + size1;

	int odd = 1;

	pRGB += (size1 - width) << 2; // * 4

	for (idy=0; idy<height; idy++){
		for(idx=0; idx<width; idx+=2){
			unsigned char r1 = (*pRGB);
			pRGB ++;
			unsigned char g1 = (*pRGB);
			pRGB ++;
			unsigned char b1 = (*pRGB);
			pRGB ++;
			unsigned char a1 = (*pRGB);
			pRGB ++;

			unsigned char r2 = (*pRGB);
			pRGB ++;
			unsigned char g2 = (*pRGB);
			pRGB ++;
			unsigned char b2 = (*pRGB);
			pRGB ++;
			unsigned char a2 = (*pRGB);
			pRGB ++;

			(*pY) = ((lookup66[r1] + lookup129[g1] + lookup25[b1]) >> 8) + 16;
			pY ++;

			(*pY) = ((lookup66[r2] + lookup129[g2] + lookup25[b2]) >> 8) + 16;
			pY ++;

			if (odd & 0x01)
			{
				(*pUV) = ((lookup_m_38[r1] + lookup_m_74[g1] + lookup112[b1]) >> 8) + 128;
				pUV ++;

				(*pUV) = ((lookup112[r1] + lookup_m_94[g1] + lookup_m_18[b1]) >> 8) + 128;
				pUV ++;
			}
		}

		odd = (!odd);
		pRGB -= (width<<3); // * 8
	}

	return 1;
}

// convert RGB565 to yuv420.
int rgb565_to_yuv420(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height)
{
	int idx;
	int idy;

	int size1 = width * height;
	int size2 = (size1 >> 2);

	short* pRGB = (short*)a_pSrcBuffRGB;
	char* pY = a_pDstBuffYUV;
	char* pV = a_pDstBuffYUV + size1;
	char* pU = pV + size2;

	int odd = 1;

	pRGB += (size1 - width);

	for (idy=0; idy<height; idy++){
		for(idx=0; idx<width; idx+=2){
			short clr1 = (*pRGB);
			pRGB ++;

			unsigned char r1 = GET_R(clr1);
			unsigned char g1 = GET_G(clr1);
			unsigned char b1 = GET_B(clr1);

			short clr2 = (*pRGB);
			pRGB ++;

			unsigned char r2 = GET_R(clr2);
			unsigned char g2 = GET_G(clr2);
			unsigned char b2 = GET_B(clr2);

			(*pY) = ((lookup66[r1] + lookup129[g1] + lookup25[b1]) >> 8) + 16;
			pY ++;

			(*pY) = ((lookup66[r2] + lookup129[g2] + lookup25[b2]) >> 8) + 16;
			pY ++;

			if (odd & 0x01)
			{
				(*pU) = ((lookup_m_38[r1] + lookup_m_74[g1] + lookup112[b1]) >> 8) + 128;
				(*pV) = ((lookup112[r1] + lookup_m_94[g1] + lookup_m_18[b1]) >> 8) + 128;
				pU++;
				pV++;
			}
		}

		odd = (!odd);
		pRGB -= (width<<1);
	}

	return 1;
}

// convert RGB565 to yuv420.
int bgr565_to_yuv420(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height)
{
	int idx;
	int idy;

	int size1 = width * height;
	int size2 = (size1 >> 2);

	short* pRGB = (short*)a_pSrcBuffRGB;
	char* pY = a_pDstBuffYUV;
	char* pV = a_pDstBuffYUV + size1;
	char* pU = pV + size2;

	int odd = 1;

	pRGB += (size1 - width);

	for (idy=0; idy<height; idy++){
		for(idx=0; idx<width; idx+=2){
			short clr1 = (*pRGB);
			pRGB ++;

			unsigned char b1 = GET_R(clr1);
			unsigned char g1 = GET_G(clr1);
			unsigned char r1 = GET_B(clr1);

			short clr2 = (*pRGB);
			pRGB ++;

			unsigned char b2 = GET_R(clr2);
			unsigned char g2 = GET_G(clr2);
			unsigned char r2 = GET_B(clr2);

			(*pY) = ((lookup66[r1] + lookup129[g1] + lookup25[b1]) >> 8) + 16;
			pY ++;

			(*pY) = ((lookup66[r2] + lookup129[g2] + lookup25[b2]) >> 8) + 16;
			pY ++;

			if (odd & 0x01)
			{
				(*pU) = ((lookup_m_38[r1] + lookup_m_74[g1] + lookup112[b1]) >> 8) + 128;
				(*pV) = ((lookup112[r1] + lookup_m_94[g1] + lookup_m_18[b1]) >> 8) + 128;
				pU++;
				pV++;
			}
		}

		odd = (!odd);
		pRGB -= (width<<1);
	}

	return 1;
}

// convert RGB8888 to yuv420p.
int rgb8888_to_yuv420(char* a_pSrcBuffRGB, char* a_pDstBuffYUV, int width, int height)
{
	int idx;
	int idy;

	int size1 = width * height;
	int size2 = (size1 >> 2);

	unsigned char* pRGB = (unsigned char*)a_pSrcBuffRGB;
	char* pY = a_pDstBuffYUV;
	char* pU = a_pDstBuffYUV + size1;
	char* pV = pU + size2;

	int odd = 1;

	pRGB += (size1 - width) << 2; // * 4

	for (idy=0; idy<height; idy++){
		for(idx=0; idx<width; idx+=2){
			unsigned char r1 = (*pRGB);
			pRGB ++;
			unsigned char g1 = (*pRGB);
			pRGB ++;
			unsigned char b1 = (*pRGB);
			pRGB ++;
			unsigned char a1 = (*pRGB);
			pRGB ++;

			unsigned char r2 = (*pRGB);
			pRGB ++;
			unsigned char g2 = (*pRGB);
			pRGB ++;
			unsigned char b2 = (*pRGB);
			pRGB ++;
			unsigned char a2 = (*pRGB);
			pRGB ++;

			(*pY) = ((lookup66[r1] + lookup129[g1] + lookup25[b1]) >> 8) + 16;
			pY ++;

			(*pY) = ((lookup66[r2] + lookup129[g2] + lookup25[b2]) >> 8) + 16;
			pY ++;

			if (odd & 0x01)
			{
				(*pU) = ((lookup_m_38[r1] + lookup_m_74[g1] + lookup112[b1]) >> 8) + 128;
				(*pV) = ((lookup112[r1] + lookup_m_94[g1] + lookup_m_18[b1]) >> 8) + 128;
				pU++;
				pV++;
			}
		}

		odd = (!odd);
		pRGB -= (width<<3); // * 8
	}

	return 1;
}


void yuv420interlaced_to_yuv420(char * yuv420sp, int srcWidth, int srcHeight, char * yuv420)
{
	if (yuv420sp == NULL ||yuv420 == NULL)
		return;

	int framesize = srcWidth * srcHeight;	// offset of U color

	int i=0, j=0, k=0, l=0;

	char*  ptrDst = yuv420;
	char*  ptrSrc = yuv420sp;

	int nSrcBuffer = 0;

	int	tile_wcount = srcWidth >> 4;
	int tile_hcount = srcHeight >> 5;

	ptrSrc = yuv420sp;
	for(i = 0; i < tile_hcount; i++){

		for(j = 0; j < tile_wcount; j++){

			ptrDst = yuv420 + (i * tile_wcount << 9) + (j << 4); //=yuv420 + (i * tile_wcount * 512) + (j * 16)

			for(k = 0; k < 32; k++){

				memcpy(ptrDst, ptrSrc, 16);
				ptrDst += srcWidth;
				ptrSrc += 16;
			}
		}
	}

	//VideoWhizErr("Buffer Convert Step2");

	char* pDstU0 = yuv420 + ((srcWidth * srcHeight * 5) >> 2);
	char* pDstV0 = yuv420 + (srcWidth * srcHeight);

	ptrSrc = yuv420sp + framesize;

	for(i = 0; i < tile_hcount; i++){

		for(j = 0; j < tile_wcount; j++){

			for(k = 0; k < 16; k++){

				int pos = ((i*srcWidth)<<3) + (j<<3) + ((k*srcWidth)>>1); //  = (i * srcWidth * 8) + (j * 8) + ((k * srcWidth)/2)
				char* pDstU = pDstU0 + pos;
				char* pDstV = pDstV0 + pos;

				// pos 0
				*pDstU = *ptrSrc; ptrSrc++;pDstU++;
				*pDstV = *ptrSrc; ptrSrc++;pDstV++;
				// pos 1
				*pDstU = *ptrSrc; ptrSrc++;pDstU++;
				*pDstV = *ptrSrc; ptrSrc++;pDstV++;
				// pos 2
				*pDstU = *ptrSrc; ptrSrc++;pDstU++;
				*pDstV = *ptrSrc; ptrSrc++;pDstV++;
				// pos 3
				*pDstU = *ptrSrc; ptrSrc++;pDstU++;
				*pDstV = *ptrSrc; ptrSrc++;pDstV++;
				// pos 4
				*pDstU = *ptrSrc; ptrSrc++;pDstU++;
				*pDstV = *ptrSrc; ptrSrc++;pDstV++;
				// pos 5
				*pDstU = *ptrSrc; ptrSrc++;pDstU++;
				*pDstV = *ptrSrc; ptrSrc++;pDstV++;
				// pos 6
				*pDstU = *ptrSrc; ptrSrc++;pDstU++;
				*pDstV = *ptrSrc; ptrSrc++;pDstV++;
				// pos 7
				*pDstU = *ptrSrc; ptrSrc++;pDstU++;
				*pDstV = *ptrSrc; ptrSrc++;pDstV++;
			}
		}
	}
}

void yuv420Sony_to_yuv420(char * yuv420src, int srcWidth, int srcHeight, char * yuv420dst)
{
	if (yuv420src == NULL ||yuv420dst == NULL)
		return;

	int framesize = srcWidth * srcHeight;	// offset of U color

	int i = 0, j = 0, k = 0, l = 0;

	char*  ptrDst1 = yuv420dst;
	char*  ptrDst2 = yuv420dst;
	char*  ptrDst3 = yuv420dst;
	char*  ptrDst4 = yuv420dst;
	char*  ptrDst5 = yuv420dst;
	char*  ptrDst6 = yuv420dst;
	char*  ptrDst7 = yuv420dst;
	char*  ptrDst8 = yuv420dst;

	char*  ptrSrc = yuv420src;
	char*  ptrSrc1 = yuv420src;
	char*  ptrSrc2 = yuv420src;
	char*  ptrSrc3 = yuv420src;
	char*  ptrSrc4 = yuv420src;
	char*  ptrSrc5 = yuv420src;
	char*  ptrSrc6 = yuv420src;
	char*  ptrSrc7 = yuv420src;
	char*  ptrSrc8 = yuv420src;

	int nSrcBuffer = 0;

	int	tile_wcount = srcWidth >> 8;
	int nRemainW = srcWidth - (tile_wcount << 8);
	int tile_hcount = srcHeight >> 6;
	int nRemainH = srcHeight - (tile_hcount << 6);
	int offset = 0;
	int offset2 = 0;

	for(i = 0; i < tile_hcount; i++)
	{
		for(j = 0; j < tile_wcount; j++)
		{
			offset = i * (srcWidth << 6) + (j << 8);
			offset2 = offset + (srcWidth << 5);

			ptrDst1 = yuv420dst + offset;
			ptrDst2 = yuv420dst + offset + 64;
			ptrDst3 = yuv420dst + offset2;
			ptrDst4 = yuv420dst + offset2 + 64;
			ptrDst5 = yuv420dst + offset2 + 128;
			ptrDst6 = yuv420dst + offset2 + 192;
			ptrDst7 = yuv420dst + offset + 128;
			ptrDst8 = yuv420dst + offset + 192;

			ptrSrc1 = ptrSrc;
			ptrSrc2 = ptrSrc1 + 2048;
			ptrSrc3 = ptrSrc2 + 2048;
			ptrSrc4 = ptrSrc3 + 2048;
			ptrSrc5 = ptrSrc4 + 2048;
			ptrSrc6 = ptrSrc5 + 2048;
			ptrSrc7 = ptrSrc6 + 2048;
			ptrSrc8 = ptrSrc7 + 2048;
			ptrSrc = ptrSrc8 + 2048;

			for(k = 0; k < 32; k++)
			{
				// 1
				memcpy(ptrDst1, ptrSrc1, 64);
				ptrDst1 += srcWidth;
				ptrSrc1 += 64;

				// 2
				memcpy(ptrDst2, ptrSrc2, 64);
				ptrDst2 += srcWidth;
				ptrSrc2 += 64;

				// 3
				memcpy(ptrDst3, ptrSrc3, 64);
				ptrDst3 += srcWidth;
				ptrSrc3 += 64;

				// 4
				memcpy(ptrDst4, ptrSrc4, 64);
				ptrDst4 += srcWidth;
				ptrSrc4 += 64;

				// 5
				memcpy(ptrDst5, ptrSrc5, 64);
				ptrDst5 += srcWidth;
				ptrSrc5 += 64;

				// 6
				memcpy(ptrDst6, ptrSrc6, 64);
				ptrDst6 += srcWidth;
				ptrSrc6 += 64;

				// 7
				memcpy(ptrDst7, ptrSrc7, 64);
				ptrDst7 += srcWidth;
				ptrSrc7 += 64;

				// 8
				memcpy(ptrDst8, ptrSrc8, 64);
				ptrDst8 += srcWidth;
				ptrSrc8 += 64;
			}
		}

		if(nRemainW > 0)
		{
			offset = i * (srcWidth << 6) + (tile_wcount << 8);
			offset2 = offset + (srcWidth << 5);
			if(nRemainW > 128)
			{
				ptrDst1 = yuv420dst + offset;
				ptrDst2 = yuv420dst + offset + 64;
				ptrDst3 = yuv420dst + offset2;
				ptrDst4 = yuv420dst + offset2 + 64;

				ptrSrc1 = ptrSrc;
				ptrSrc2 = ptrSrc1 + 2048;
				ptrSrc3 = ptrSrc2 + 2048;
				ptrSrc4 = ptrSrc3 + 2048;
				ptrSrc = ptrSrc4 + 2048;

				for(k = 0; k < 32; k++)
				{
					// 1
					memcpy(ptrDst1, ptrSrc1, 64);
					ptrDst1 += srcWidth;
					ptrSrc1 += 64;
					// 2
					memcpy(ptrDst2, ptrSrc2, 64);
					ptrDst2 += srcWidth;
					ptrSrc2 += 64;
					// 3
					memcpy(ptrDst3, ptrSrc3, 64);
					ptrDst3 += srcWidth;
					ptrSrc3 += 64;
					// 4
					memcpy(ptrDst4, ptrSrc4, 64);
					ptrDst4 += srcWidth;
					ptrSrc4 += 64;
				}
				int nSubRemainW = nRemainW - 128;
				if(nSubRemainW > 64)
				{
					nSubRemainW -= 64;

					ptrDst5 = yuv420dst + offset2 + 128;
					ptrDst6 = yuv420dst + offset2 + 192;
					ptrDst7 = yuv420dst + offset + 128;
					ptrDst8 = yuv420dst + offset + 192;

					ptrSrc5 = ptrSrc;
					ptrSrc6 = ptrSrc5 + 2048;
					ptrSrc7 = ptrSrc6 + 2048;
					ptrSrc8 = ptrSrc7 + 2048;
					ptrSrc = ptrSrc8 + 2048;

					for(k = 0; k < 32; k++)
					{
						// 5
						memcpy(ptrDst5, ptrSrc5, 64);
						ptrDst5 += srcWidth;
						ptrSrc5 += 64;
						// 6
						memcpy(ptrDst6, ptrSrc6, nSubRemainW);
						ptrDst6 += srcWidth;
						ptrSrc6 += 64;
						// 7
						memcpy(ptrDst7, ptrSrc7, 64);
						ptrDst7 += srcWidth;
						ptrSrc7 += 64;
						// 8
						memcpy(ptrDst8, ptrSrc8, nSubRemainW);
						ptrDst8 += srcWidth;
						ptrSrc8 += 64;
					}
				}
				else
				{
					ptrDst5 = yuv420dst + offset2 + 128;
					ptrDst7 = yuv420dst + offset + 128;

					ptrSrc5 = ptrSrc;
					ptrSrc6 = ptrSrc5 + 2048;
					ptrSrc7 = ptrSrc6 + 2048;
					ptrSrc8 = ptrSrc7 + 2048;
					ptrSrc = ptrSrc8 + 2048;

					for(k = 0; k < 32; k++)
					{
						// 5
						memcpy(ptrDst5, ptrSrc5, nSubRemainW);
						ptrDst5 += srcWidth;
						ptrSrc5 += 64;
						// 6
						//ptrSrc6 += 64;
						// 7
						memcpy(ptrDst7, ptrSrc7, nSubRemainW);
						ptrDst7 += srcWidth;
						ptrSrc7 += 64;
						// 8
						//ptrSrc8 += 64;
					}
				}
			}
			else // if(nRemainW <= 128)
			{
				if(nRemainW > 64)
				{
					int nSubRemainW = nRemainW - 64;

					ptrDst1 = yuv420dst + offset;
					ptrDst2 = yuv420dst + offset + 64;
					ptrDst3 = yuv420dst + offset2;
					ptrDst4 = yuv420dst + offset2 + 64;

					ptrSrc1 = ptrSrc;
					ptrSrc2 = ptrSrc1 + 2048;
					ptrSrc3 = ptrSrc2 + 2048;
					ptrSrc4 = ptrSrc3 + 2048;
					ptrSrc = ptrSrc4 + 2048;

					for(k = 0; k < 32; k++)
					{
						// 1
						memcpy(ptrDst1, ptrSrc1, 64);
						ptrDst1 += srcWidth;
						ptrSrc1 += 64;
						// 2
						memcpy(ptrDst2, ptrSrc2, nSubRemainW);
						ptrDst2 += srcWidth;
						ptrSrc2 += 64;
						// 3
						memcpy(ptrDst3, ptrSrc3, 64);
						ptrDst3 += srcWidth;
						ptrSrc3 += 64;
						// 4
						memcpy(ptrDst4, ptrSrc4, nSubRemainW);
						ptrDst4 += srcWidth;
						ptrSrc4 += 64;
					}
				}
				else
				{
					ptrDst1 = yuv420dst + offset;
					ptrDst3 = yuv420dst + offset2;

					ptrSrc1 = ptrSrc;
					ptrSrc2 = ptrSrc1 + 2048;
					ptrSrc3 = ptrSrc2 + 2048;
					ptrSrc4 = ptrSrc3 + 2048;
					ptrSrc = ptrSrc4 + 2048;

					for(k = 0; k < 32; k++)
					{
						// 1
						memcpy(ptrDst1, ptrSrc1, nRemainW);
						ptrDst1 += srcWidth;
						ptrSrc1 += 64;
						// 2
						//ptrSrc2 += 64;
						// 3
						memcpy(ptrDst3, ptrSrc3, nRemainW);
						ptrDst3 += srcWidth;
						ptrSrc3 += 64;
						// 4
						//ptrSrc4 += 64;
					}
				}
			}
		}
	}

	if(nRemainH > 0)
	{
		if(nRemainH > 32)
		{
			int nSubRemainH = nRemainH - 32;
			for(j = 0; j < tile_wcount; j++)
			{
				offset = tile_hcount * (srcWidth << 6) + (j << 8);
				offset2 = offset + (srcWidth << 5);

				ptrDst1 = yuv420dst + offset;
				ptrDst2 = yuv420dst + offset + 64;
				ptrDst3 = yuv420dst + offset2;
				ptrDst4 = yuv420dst + offset2 + 64;
				ptrDst5 = yuv420dst + offset2 + 128;
				ptrDst6 = yuv420dst + offset2 + 192;
				ptrDst7 = yuv420dst + offset + 128;
				ptrDst8 = yuv420dst + offset + 192;

				ptrSrc1 = ptrSrc;
				ptrSrc2 = ptrSrc1 + 2048;
				ptrSrc3 = ptrSrc2 + 2048;
				ptrSrc4 = ptrSrc3 + 2048;
				ptrSrc5 = ptrSrc4 + 2048;
				ptrSrc6 = ptrSrc5 + 2048;
				ptrSrc7 = ptrSrc6 + 2048;
				ptrSrc8 = ptrSrc7 + 2048;
				ptrSrc = ptrSrc8 + 2048;

				for(k = 0; k < 32; k++)
				{
					// 1
					memcpy(ptrDst1, ptrSrc1, 64);
					ptrDst1 += srcWidth;
					ptrSrc1 += 64;
					// 2
					memcpy(ptrDst2, ptrSrc2, 64);
					ptrDst2 += srcWidth;
					ptrSrc2 += 64;
					if(k < nSubRemainH)
					{
						// 3
						memcpy(ptrDst3, ptrSrc3, 64);
						ptrDst3 += srcWidth;
						ptrSrc3 += 64;
						// 4
						memcpy(ptrDst4, ptrSrc4, 64);
						ptrDst4 += srcWidth;
						ptrSrc4 += 64;
						// 5
						memcpy(ptrDst5, ptrSrc5, 64);
						ptrDst5 += srcWidth;
						ptrSrc5 += 64;
						// 6
						memcpy(ptrDst6, ptrSrc6, 64);
						ptrDst6 += srcWidth;
						ptrSrc6 += 64;
					}
					else
					{
						ptrSrc3 += 64;
						ptrSrc4 += 64;
						ptrSrc5 += 64;
						ptrSrc6 += 64;
					}
					// 7
					memcpy(ptrDst7, ptrSrc7, 64);
					ptrDst7 += srcWidth;
					ptrSrc7 += 64;
					// 8
					memcpy(ptrDst8, ptrSrc8, 64);
					ptrDst8 += srcWidth;
					ptrSrc8 += 64;
				}
			}

			if(nRemainW > 0)
			{
				offset = tile_hcount * (srcWidth << 6) + (tile_wcount << 8);
				offset2 = offset + (srcWidth << 5);
				if(nRemainW > 128)
				{
					ptrDst1 = yuv420dst + offset;
					ptrDst2 = yuv420dst + offset + 64;
					ptrDst3 = yuv420dst + offset2;
					ptrDst4 = yuv420dst + offset2 + 64;

					ptrSrc1 = ptrSrc;
					ptrSrc2 = ptrSrc1 + 2048;
					ptrSrc3 = ptrSrc2 + 2048;
					ptrSrc4 = ptrSrc3 + 2048;
					ptrSrc = ptrSrc4 + 2048;

					for(k = 0; k < 32; k++)
					{
						// 1
						memcpy(ptrDst1, ptrSrc1, 64);
						ptrDst1 += srcWidth;
						ptrSrc1 += 64;
						// 2
						memcpy(ptrDst2, ptrSrc2, 64);
						ptrDst2 += srcWidth;
						ptrSrc2 += 64;
						if(k < nSubRemainH)
						{
							// 3
							memcpy(ptrDst3, ptrSrc3, 64);
							ptrDst3 += srcWidth;
							ptrSrc3 += 64;
							// 4
							memcpy(ptrDst4, ptrSrc4, 64);
							ptrDst4 += srcWidth;
							ptrSrc4 += 64;
						}
						else
						{
							ptrSrc3 += 64;
							ptrSrc4 += 64;
						}
					}

					int nSubRemainW = nRemainW - 128;
					if(nSubRemainW > 64)
					{
						nSubRemainW -= 64;

						ptrDst5 = yuv420dst + offset2 + 128;
						ptrDst6 = yuv420dst + offset2 + 192;
						ptrDst7 = yuv420dst + offset + 128;
						ptrDst8 = yuv420dst + offset + 192;

						ptrSrc5 = ptrSrc;
						ptrSrc6 = ptrSrc5 + 2048;
						ptrSrc7 = ptrSrc6 + 2048;
						ptrSrc8 = ptrSrc7 + 2048;
						ptrSrc = ptrSrc8 + 2048;

						for(k = 0; k < 32; k++)
						{
							if(k < nSubRemainH)
							{
								// 5
								memcpy(ptrDst5, ptrSrc5, 64);
								ptrDst5 += srcWidth;
								ptrSrc5 += 64;
								// 6
								memcpy(ptrDst6, ptrSrc6, nSubRemainW);
								ptrDst6 += srcWidth;
								ptrSrc6 += 64;
							}
							else
							{
								ptrSrc5 += 64;
								ptrSrc6 += 64;
							}
							// 7
							memcpy(ptrDst7, ptrSrc7, 64);
							ptrDst7 += srcWidth;
							ptrSrc7 += 64;
							// 8
							memcpy(ptrDst8, ptrSrc8, nSubRemainW);
							ptrDst8 += srcWidth;
							ptrSrc8 += 64;
						};
					}
					else
					{
						ptrDst5 = yuv420dst + offset2 + 128;
						ptrDst7 = yuv420dst + offset + 128;

						ptrSrc5 = ptrSrc;
						ptrSrc6 = ptrSrc5 + 2048;
						ptrSrc7 = ptrSrc6 + 2048;
						ptrSrc8 = ptrSrc7 + 2048;
						ptrSrc = ptrSrc8 + 2048;

						for(k = 0; k < 32; k++)
						{
							if(k < nSubRemainH)
							{
								// 5
								memcpy(ptrDst5, ptrSrc5, nSubRemainW);
								ptrDst5 += srcWidth;
								ptrSrc5 += 64;
							}
							else
							{
								ptrSrc5 += 64;
							}
							// 6
							//ptrSrc6 += 64;
							// 7
							memcpy(ptrDst7, ptrSrc7, nSubRemainW);
							ptrDst7 += srcWidth;
							ptrSrc7 += 64;
							// 8
							//ptrSrc8 += 64;
						}
					}
				}
				else // if(nRemainW <= 128)
				{
					if(nRemainW > 64)
					{
						int nSubRemainW = nRemainW - 64;

						ptrDst1 = yuv420dst + offset;
						ptrDst2 = yuv420dst + offset + 64;
						ptrDst3 = yuv420dst + offset2;
						ptrDst4 = yuv420dst + offset2 + 64;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc3 = ptrSrc2 + 2048;
						ptrSrc4 = ptrSrc3 + 2048;
						ptrSrc = ptrSrc4 + 2048;

						for(k = 0; k < 32; k++)
						{
							// 1
							memcpy(ptrDst1, ptrSrc1, 64);
							ptrDst1 += srcWidth;
							ptrSrc1 += 64;
							// 2
							memcpy(ptrDst2, ptrSrc2, nSubRemainW);
							ptrDst2 += srcWidth;
							ptrSrc2 += 64;
							if(k < nSubRemainH)
							{
								// 3
								memcpy(ptrDst3, ptrSrc3, 64);
								ptrDst3 += srcWidth;
								ptrSrc3 += 64;
								// 4
								memcpy(ptrDst4, ptrSrc4, nSubRemainW);
								ptrDst4 += srcWidth;
								ptrSrc4 += 64;
							}
							else
							{
								ptrSrc3 += 64;
								ptrSrc4 += 64;
							}
						}
					}
					else
					{
						ptrDst1 = yuv420dst + offset;
						ptrDst3 = yuv420dst + offset2;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc3 = ptrSrc2 + 2048;
						ptrSrc4 = ptrSrc3 + 2048;
						ptrSrc = ptrSrc4 + 2048;

						for(k = 0; k < 32; k++)
						{
							// 1
							memcpy(ptrDst1, ptrSrc1, nRemainW);
							ptrDst1 += srcWidth;
							ptrSrc1 += 64;
							// 2
							//ptrSrc2 += 64;
							if(k < nSubRemainH)
							{
								// 3
								memcpy(ptrDst3, ptrSrc3, nRemainW);
								ptrDst3 += srcWidth;
								ptrSrc3 += 64;
							}
							else
							{
								ptrSrc3 += 64;
							}
							// 4
							//ptrSrc4 += 64;
						}
					}
				}
			}
		}
		else
		{
			int nSubRemainH = nRemainH;
			for(j = 0; j < tile_wcount; j++)
			{
				offset = tile_hcount * (srcWidth << 6) + (j << 8);

				ptrDst1 = yuv420dst + offset;
				ptrDst2 = yuv420dst + offset + 64;
				ptrDst3 = yuv420dst + offset + 128;
				ptrDst4 = yuv420dst + offset + 192;

				ptrSrc1 = ptrSrc;
				ptrSrc2 = ptrSrc1 + 2048;
				ptrSrc3 = ptrSrc2 + 2048;
				ptrSrc4 = ptrSrc3 + 2048;
				ptrSrc = ptrSrc4 + 2048;

				for(k = 0; k < nSubRemainH; k++)
				{
					// 1
					memcpy(ptrDst1, ptrSrc1, 64);
					ptrDst1 += srcWidth;
					ptrSrc1 += 64;
					// 2
					memcpy(ptrDst2, ptrSrc2, 64);
					ptrDst2 += srcWidth;
					ptrSrc2 += 64;
					// 3
					memcpy(ptrDst3, ptrSrc3, 64);
					ptrDst3 += srcWidth;
					ptrSrc3 += 64;
					// 4
					memcpy(ptrDst4, ptrSrc4, 64);
					ptrDst4 += srcWidth;
					ptrSrc4 += 64;
				}
			}

			if(nRemainW > 0)
			{
				offset = tile_hcount * (srcWidth << 6) + (tile_wcount << 8);
				if(nRemainW > 128)
				{
					ptrDst1 = yuv420dst + offset;
					ptrDst2 = yuv420dst + offset + 64;

					ptrSrc1 = ptrSrc;
					ptrSrc2 = ptrSrc1 + 2048;
					ptrSrc = ptrSrc2 + 2048;

					for(k = 0; k < nSubRemainH; k++)
					{
						// 1
						memcpy(ptrDst1, ptrSrc1, 64);
						ptrDst1 += srcWidth;
						ptrSrc1 += 64;
						// 2
						memcpy(ptrDst2, ptrSrc2, 64);
						ptrDst2 += srcWidth;
						ptrSrc2 += 64;
					}

					int nSubRemainW = nRemainW - 128;
					if(nSubRemainW > 64)
					{
						nSubRemainW -= 64;

						ptrDst1 = yuv420dst + offset + 128;
						ptrDst2 = yuv420dst + offset + 192;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc = ptrSrc2 + 2048;

						for(k = 0; k < nSubRemainH; k++)
						{
							// 1
							memcpy(ptrDst1, ptrSrc1, 64);
							ptrDst1 += srcWidth;
							ptrSrc1 += 64;
							// 2
							memcpy(ptrDst2, ptrSrc2, nSubRemainW);
							ptrDst2 += srcWidth;
							ptrSrc2 += 64;
						}
					}
					else
					{
						ptrDst1 = yuv420dst + offset + 128;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc = ptrSrc2 + 2048;

						for(k = 0; k < nSubRemainH; k++)
						{
							// 1
							memcpy(ptrDst1, ptrSrc1, nSubRemainW);
							ptrDst1 += srcWidth;
							ptrSrc1 += 64;

						}
					}
				}
				else // if(nRemainW <= 128)
				{
					if(nRemainW > 64)
					{
						int nSubRemainW = nRemainW - 64;

						ptrDst1 = yuv420dst + offset;
						ptrDst2 = yuv420dst + offset + 64;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc = ptrSrc2 + 2048;

						for(k = 0; k < nSubRemainH; k++)
						{
							// 1
							memcpy(ptrDst1, ptrSrc1, 64);
							ptrDst1 += srcWidth;
							ptrSrc1 += 64;
							// 2
							memcpy(ptrDst2, ptrSrc2, nSubRemainW);
							ptrDst2 += srcWidth;
							ptrSrc2 += 64;
						}
					}
					else
					{
						ptrDst1 = yuv420dst + offset;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc = ptrSrc2 + 2048;

						for(k = 0; k < nSubRemainH; k++)
						{
							// 1
							memcpy(ptrDst1, ptrSrc1, nRemainW);
							ptrDst1 += srcWidth;
							ptrSrc1 += 64;
						}
					}
					ptrSrc += 4096;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	char* pDstU0 = yuv420dst + ((framesize * 5) >> 2);
	char* pDstU1 = yuv420dst + ((framesize * 5) >> 2);
	char* pDstU2 = yuv420dst + ((framesize * 5) >> 2);
	char* pDstU3 = yuv420dst + ((framesize * 5) >> 2);
	char* pDstU4 = yuv420dst + ((framesize * 5) >> 2);
	char* pDstU5 = yuv420dst + ((framesize * 5) >> 2);
	char* pDstU6 = yuv420dst + ((framesize * 5) >> 2);
	char* pDstU7 = yuv420dst + ((framesize * 5) >> 2);
	char* pDstU8 = yuv420dst + ((framesize * 5) >> 2);

	char* pDstV0 = yuv420dst + framesize;
	char* pDstV1 = yuv420dst + framesize;
	char* pDstV2 = yuv420dst + framesize;
	char* pDstV3 = yuv420dst + framesize;
	char* pDstV4 = yuv420dst + framesize;
	char* pDstV5 = yuv420dst + framesize;
	char* pDstV6 = yuv420dst + framesize;
	char* pDstV7 = yuv420dst + framesize;
	char* pDstV8 = yuv420dst + framesize;

	int nHalfWidth = srcWidth >> 1;
	tile_hcount = srcHeight >> 7;
	nRemainH = (srcHeight >> 1) - (tile_hcount << 6);

	for(i = 0; i < tile_hcount; i++)
	{
		for(j = 0; j < tile_wcount; j++)
		{
			offset = i * (srcWidth << 5) + (j << 7);
			offset2 = offset + (srcWidth << 4);

			ptrSrc1 = ptrSrc;
			ptrSrc2 = ptrSrc1 + 2048;
			ptrSrc3 = ptrSrc2 + 2048;
			ptrSrc4 = ptrSrc3 + 2048;
			ptrSrc5 = ptrSrc4 + 2048;
			ptrSrc6 = ptrSrc5 + 2048;
			ptrSrc7 = ptrSrc6 + 2048;
			ptrSrc8 = ptrSrc7 + 2048;
			ptrSrc = ptrSrc8 + 2048;

			for(k = 0; k < 32; k++)
			{
				pDstU1 = pDstU0 + offset + k * nHalfWidth;
				pDstV1 = pDstV0 + offset + k * nHalfWidth;
				pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
				pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;
				pDstU3 = pDstU0 + offset2 + k * nHalfWidth;
				pDstV3 = pDstV0 + offset2 + k * nHalfWidth;
				pDstU4 = pDstU0 + offset2 + 32 + k * nHalfWidth;
				pDstV4 = pDstV0 + offset2 + 32 + k * nHalfWidth;
				pDstU5 = pDstU0 + offset2 + 64 + k * nHalfWidth;
				pDstV5 = pDstV0 + offset2 + 64 + k * nHalfWidth;
				pDstU6 = pDstU0 + offset2 + 96 + k * nHalfWidth;
				pDstV6 = pDstV0 + offset2 + 96 + k * nHalfWidth;
				pDstU7 = pDstU0 + offset + 64 + k * nHalfWidth;
				pDstV7 = pDstV0 + offset + 64 + k * nHalfWidth;
				pDstU8 = pDstU0 + offset + 96 + k * nHalfWidth;
				pDstV8 = pDstV0 + offset + 96 + k * nHalfWidth;

				for(l = 0; l < 32; l++) // 64 / 2
				{
					// 1
					*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
					*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
					// 2
					*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
					*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
					// 3
					*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
					*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
					// 4
					*(pDstU4 + l) = *ptrSrc4;ptrSrc4++;
					*(pDstV4 + l) = *ptrSrc4;ptrSrc4++;
					// 5
					*(pDstU5 + l) = *ptrSrc5;ptrSrc5++;
					*(pDstV5 + l) = *ptrSrc5;ptrSrc5++;
					// 6
					*(pDstU6 + l) = *ptrSrc6;ptrSrc6++;
					*(pDstV6 + l) = *ptrSrc6;ptrSrc6++;
					// 7
					*(pDstU7 + l) = *ptrSrc7;ptrSrc7++;
					*(pDstV7 + l) = *ptrSrc7;ptrSrc7++;
					// 8
					*(pDstU8 + l) = *ptrSrc8;ptrSrc8++;
					*(pDstV8 + l) = *ptrSrc8;ptrSrc8++;
				}
			}
		}

		if(nRemainW > 0)
		{
			offset = i * (srcWidth << 5) + (tile_wcount << 7);
			offset2 = offset + (srcWidth << 4);
			if(nRemainW > 128)
			{
				ptrSrc1 = ptrSrc;
				ptrSrc2 = ptrSrc1 + 2048;
				ptrSrc3 = ptrSrc2 + 2048;
				ptrSrc4 = ptrSrc3 + 2048;
				ptrSrc = ptrSrc4 + 2048;

				for(k = 0; k < 32; k++)
				{
					pDstU1 = pDstU0 + offset + k * nHalfWidth;
					pDstV1 = pDstV0 + offset + k * nHalfWidth;
					pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
					pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;
					pDstU3 = pDstU0 + offset2 + k * nHalfWidth;
					pDstV3 = pDstV0 + offset2 + k * nHalfWidth;
					pDstU4 = pDstU0 + offset2 + 32 + k * nHalfWidth;
					pDstV4 = pDstV0 + offset2 + 32 + k * nHalfWidth;
					for(l = 0; l < 32; l++) // 64 / 2
					{
						// 1
						*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
						*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
						// 2
						*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
						*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
						// 3
						*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
						*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
						// 4
						*(pDstU4 + l) = *ptrSrc4;ptrSrc4++;
						*(pDstV4 + l) = *ptrSrc4;ptrSrc4++;
					}
				}

				int nSubRemainW = nRemainW - 128;
				int nLimitW = nSubRemainW >> 1;
				if(nSubRemainW > 64)
				{
					nSubRemainW -= 64;

					ptrSrc5 = ptrSrc;
					ptrSrc6 = ptrSrc5 + 2048;
					ptrSrc7 = ptrSrc6 + 2048;
					ptrSrc8 = ptrSrc7 + 2048;
					ptrSrc = ptrSrc8 + 2048;

					for(k = 0; k < 32; k++)
					{
						pDstU5 = pDstU0 + offset2 + 64 + k * nHalfWidth;
						pDstV5 = pDstV0 + offset2 + 64 + k * nHalfWidth;
						pDstU6 = pDstU0 + offset2 + 96 + k * nHalfWidth;
						pDstV6 = pDstV0 + offset2 + 96 + k * nHalfWidth;
						pDstU7 = pDstU0 + offset + 64 + k * nHalfWidth;
						pDstV7 = pDstV0 + offset + 64 + k * nHalfWidth;
						pDstU8 = pDstU0 + offset + 96 + k * nHalfWidth;
						pDstV8 = pDstV0 + offset + 96 + k * nHalfWidth;

						for(l = 0; l < 32; l++) // 64 / 2
						{
							// 5
							*(pDstU5 + l) = *ptrSrc5;ptrSrc5++;
							*(pDstV5 + l) = *ptrSrc5;ptrSrc5++;
							// 7
							*(pDstU7 + l) = *ptrSrc7;ptrSrc7++;
							*(pDstV7 + l) = *ptrSrc7;ptrSrc7++;
							if(l < nLimitW)
							{
								// 6
								*(pDstU6 + l) = *ptrSrc6;ptrSrc6++;
								*(pDstV6 + l) = *ptrSrc6;ptrSrc6++;
								// 8
								*(pDstU8 + l) = *ptrSrc8;ptrSrc8++;
								*(pDstV8 + l) = *ptrSrc8;ptrSrc8++;
							}
							else
							{
								ptrSrc6 += 2;
								ptrSrc8 += 2;
							}
						}
					}
				}
				else
				{
					ptrSrc5 = ptrSrc;
					ptrSrc6 = ptrSrc5 + 2048;
					ptrSrc7 = ptrSrc6 + 2048;
					ptrSrc8 = ptrSrc7 + 2048;
					ptrSrc = ptrSrc8 + 2048;

					for(k = 0; k < 32; k++)
					{
						pDstU5 = pDstU0 + offset2 + 64 + k * nHalfWidth;
						pDstV5 = pDstV0 + offset2 + 64 + k * nHalfWidth;
						pDstU7 = pDstU0 + offset + 64 + k * nHalfWidth;
						pDstV7 = pDstV0 + offset + 64 + k * nHalfWidth;

						for(l = 0; l < nLimitW; l++) // nSubRemainW / 2
						{
							// 5
							*(pDstU5 + l) = *ptrSrc5;ptrSrc5++;
							*(pDstV5 + l) = *ptrSrc5;ptrSrc5++;
							// 7
							*(pDstU7 + l) = *ptrSrc7;ptrSrc7++;
							*(pDstV7 + l) = *ptrSrc7;ptrSrc7++;
						}
						ptrSrc5 += 64 - nSubRemainW;
						ptrSrc7 += 64 - nSubRemainW;
					}
				}
			}
			else // if(nRemainW <= 128)
			{
				ptrSrc1 = ptrSrc;
				ptrSrc2 = ptrSrc1 + 2048;
				ptrSrc3 = ptrSrc2 + 2048;
				ptrSrc4 = ptrSrc3 + 2048;
				ptrSrc = ptrSrc4 + 2048;

				if(nRemainW > 64)
				{
					int nSubRemainW = nRemainW - 64;
					int nLimitW = nSubRemainW >> 1;
					for(k = 0; k < 32; k++)
					{
						pDstU1 = pDstU0 + offset + k * nHalfWidth;
						pDstV1 = pDstV0 + offset + k * nHalfWidth;
						pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
						pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;
						pDstU3 = pDstU0 + offset2 + k * nHalfWidth;
						pDstV3 = pDstV0 + offset2 + k * nHalfWidth;
						pDstU4 = pDstU0 + offset2 + 32 + k * nHalfWidth;
						pDstV4 = pDstV0 + offset2 + 32 + k * nHalfWidth;

						for(l = 0; l < 32; l++) // 64 / 2
						{
							// 1
							*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
							*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
							// 3
							*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
							*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
							if(l < nLimitW)
							{
								// 2
								*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
								*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
								// 4
								*(pDstU4 + l) = *ptrSrc4;ptrSrc4++;
								*(pDstV4 + l) = *ptrSrc4;ptrSrc4++;
							}
							else
							{
								ptrSrc2 += 2;
								ptrSrc4 += 2;
							}
						}
					}
				}
				else
				{
					int nLimitW = nRemainW >> 1;

					for(k = 0; k < 32; k++)
					{
						pDstU1 = pDstU0 + offset + k * nHalfWidth;
						pDstV1 = pDstV0 + offset + k * nHalfWidth;
						pDstU3 = pDstU0 + offset2 + k * nHalfWidth;
						pDstV3 = pDstV0 + offset2 + k * nHalfWidth;

						for(l = 0; l < nLimitW; l++) // nRemainW / 2
						{
							// 1
							*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
							*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
							// 3
							*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
							*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
						}
						ptrSrc1 += 64 - nRemainW;
						ptrSrc3 += 64 - nRemainW;
					}
				}
			}
		}
	}

	if(nRemainH > 0)
	{
		if(nRemainH > 32)
		{
			int nSubRemainH = nRemainH - 32;
			for(j = 0; j < tile_wcount; j++)
			{
				offset = tile_hcount * (srcWidth << 5) + (j << 7);
				offset2 = offset + (srcWidth << 4);

				ptrSrc1 = ptrSrc;
				ptrSrc2 = ptrSrc1 + 2048;
				ptrSrc3 = ptrSrc2 + 2048;
				ptrSrc4 = ptrSrc3 + 2048;
				ptrSrc5 = ptrSrc4 + 2048;
				ptrSrc6 = ptrSrc5 + 2048;
				ptrSrc7 = ptrSrc6 + 2048;
				ptrSrc8 = ptrSrc7 + 2048;
				ptrSrc = ptrSrc8 + 2048;

				for(k = 0; k < 32; k++)
				{
					pDstU1 = pDstU0 + offset + k * nHalfWidth;
					pDstV1 = pDstV0 + offset + k * nHalfWidth;
					pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
					pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;
					pDstU3 = pDstU0 + offset2 + k * nHalfWidth;
					pDstV3 = pDstV0 + offset2 + k * nHalfWidth;
					pDstU4 = pDstU0 + offset2 + 32 + k * nHalfWidth;
					pDstV4 = pDstV0 + offset2 + 32 + k * nHalfWidth;
					pDstU5 = pDstU0 + offset2 + 64 + k * nHalfWidth;
					pDstV5 = pDstV0 + offset2 + 64 + k * nHalfWidth;
					pDstU6 = pDstU0 + offset2 + 96 + k * nHalfWidth;
					pDstV6 = pDstV0 + offset2 + 96 + k * nHalfWidth;
					pDstU7 = pDstU0 + offset + 64 + k * nHalfWidth;
					pDstV7 = pDstV0 + offset + 64 + k * nHalfWidth;
					pDstU8 = pDstU0 + offset + 96 + k * nHalfWidth;
					pDstV8 = pDstV0 + offset + 96 + k * nHalfWidth;

					for(l = 0; l < 32; l++) // 64 / 2
					{
						// 1
						*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
						*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
						// 2
						*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
						*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
						// 7
						*(pDstU7 + l) = *ptrSrc7;ptrSrc7++;
						*(pDstV7 + l) = *ptrSrc7;ptrSrc7++;
						// 8
						*(pDstU8 + l) = *ptrSrc8;ptrSrc8++;
						*(pDstV8 + l) = *ptrSrc8;ptrSrc8++;

					}
					if(k < nSubRemainH)
					{
						for(l = 0; l < 32; l++) // 64 / 2
						{
							// 3
							*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
							*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
							// 4
							*(pDstU4 + l) = *ptrSrc4;ptrSrc4++;
							*(pDstV4 + l) = *ptrSrc4;ptrSrc4++;
							// 5
							*(pDstU5 + l) = *ptrSrc5;ptrSrc5++;
							*(pDstV5 + l) = *ptrSrc5;ptrSrc5++;
							// 6
							*(pDstU6 + l) = *ptrSrc6;ptrSrc6++;
							*(pDstV6 + l) = *ptrSrc6;ptrSrc6++;
						}
					}
					else
					{
						ptrSrc3 += 64;
						ptrSrc4 += 64;
						ptrSrc5 += 64;
						ptrSrc6 += 64;
					}
				}
			}

			if(nRemainW > 0)
			{
				offset = tile_hcount * (srcWidth << 5) + (tile_wcount << 7);
				offset2 = offset + (srcWidth << 4);
				if(nRemainW > 128)
				{
					ptrSrc1 = ptrSrc;
					ptrSrc2 = ptrSrc1 + 2048;
					ptrSrc3 = ptrSrc2 + 2048;
					ptrSrc4 = ptrSrc3 + 2048;
					ptrSrc = ptrSrc4 + 2048;

					for(k = 0; k < 32; k++)
					{
						pDstU1 = pDstU0 + offset + k * nHalfWidth;
						pDstV1 = pDstV0 + offset + k * nHalfWidth;
						pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
						pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;
						pDstU3 = pDstU0 + offset2 + k * nHalfWidth;
						pDstV3 = pDstV0 + offset2 + k * nHalfWidth;
						pDstU4 = pDstU0 + offset2 + 32 + k * nHalfWidth;
						pDstV4 = pDstV0 + offset2 + 32 + k * nHalfWidth;

						for(l = 0; l < 32; l++) // 64 / 2
						{
							// 1
							*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
							*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
							// 2
							*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
							*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
						}
						if(k < nSubRemainH)
						{
							for(l = 0; l < 32; l++) // 64 / 2
							{
								// 3
								*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
								*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
								// 4
								*(pDstU4 + l) = *ptrSrc4;ptrSrc4++;
								*(pDstV4 + l) = *ptrSrc4;ptrSrc4++;
							}
						}
						else
						{
							ptrSrc3 += 64;
							ptrSrc4 += 64;
						}
					}

					int nSubRemainW = nRemainW - 128;
					if(nSubRemainW > 64)
					{
						nSubRemainW -= 64;
						int nLimitW = nSubRemainW >> 1;

						ptrSrc5 = ptrSrc;
						ptrSrc6 = ptrSrc5 + 2048;
						ptrSrc7 = ptrSrc6 + 2048;
						ptrSrc8 = ptrSrc7 + 2048;
						ptrSrc = ptrSrc8 + 2048;

						for(k = 0; k < 32; k++)
						{
							pDstU5 = pDstU0 + offset2 + 64 + k * nHalfWidth;
							pDstV5 = pDstV0 + offset2 + 64 + k * nHalfWidth;
							pDstU6 = pDstU0 + offset2 + 96 + k * nHalfWidth;
							pDstV6 = pDstV0 + offset2 + 96 + k * nHalfWidth;
							pDstU7 = pDstU0 + offset + 64 + k * nHalfWidth;
							pDstV7 = pDstV0 + offset + 64 + k * nHalfWidth;
							pDstU8 = pDstU0 + offset + 96 + k * nHalfWidth;
							pDstV8 = pDstV0 + offset + 96 + k * nHalfWidth;

							if(k < nSubRemainH)
							{
								for(l = 0; l < 32; l++) // 64 / 2
								{
									// 5
									*(pDstU5 + l) = *ptrSrc5;ptrSrc5++;
									*(pDstV5 + l) = *ptrSrc5;ptrSrc5++;
									if(l < nLimitW)
									{
										// 6
										*(pDstU6 + l) = *ptrSrc6;ptrSrc6++;
										*(pDstV6 + l) = *ptrSrc6;ptrSrc6++;
									}
									else
									{
										ptrSrc6 += 2;
									}
								}
							}
							else
							{
								ptrSrc5 += 64;
								ptrSrc6 += 64;
							}
							for(l = 0; l < 32; l++) // 64 / 2
							{
								// 7
								*(pDstU7 + l) = *ptrSrc7;ptrSrc7++;
								*(pDstV7 + l) = *ptrSrc7;ptrSrc7++;
								if(l < nLimitW)
								{
									// 8
									*(pDstU8 + l) = *ptrSrc8;ptrSrc8++;
									*(pDstV8 + l) = *ptrSrc8;ptrSrc8++;
								}
								else
								{
									ptrSrc8 += 2;
								}
							}
						}
					}
					else
					{
						int nLimitW = nSubRemainW >> 1;

						ptrSrc5 = ptrSrc;
						ptrSrc6 = ptrSrc5 + 2048;
						ptrSrc7 = ptrSrc6 + 2048;
						ptrSrc8 = ptrSrc7 + 2048;
						ptrSrc = ptrSrc8 + 2048;

						for(k = 0; k < 32; k++)
						{
							pDstU5 = pDstU0 + offset2 + 64 + k * nHalfWidth;
							pDstV5 = pDstV0 + offset2 + 64 + k * nHalfWidth;
							pDstU7 = pDstU0 + offset + 64 + k * nHalfWidth;
							pDstV7 = pDstV0 + offset + 64 + k * nHalfWidth;

							if(k < nSubRemainH)
							{
								for(l = 0; l < nLimitW; l++) // nSubRemainW / 2
								{
									// 5
									*(pDstU5 + l) = *ptrSrc5;ptrSrc5++;
									*(pDstV5 + l) = *ptrSrc5;ptrSrc5++;
								}
								ptrSrc5 += 64 - nSubRemainW;
							}
							else
							{
								ptrSrc5 += 64;
							}
							for(l = 0; l < nLimitW; l++) // nSubRemainW / 2
							{
								// 7
								*(pDstU7 + l) = *ptrSrc7;ptrSrc7++;
								*(pDstV7 + l) = *ptrSrc7;ptrSrc7++;
							}
							ptrSrc7 += 64 - nSubRemainW;
						}
					}
				}
				else // if(nRemainW <= 128)
				{
					if(nRemainW > 64)
					{
						int nSubRemainW = nRemainW - 64;
						int nLimitW = nSubRemainW >> 1;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc3 = ptrSrc2 + 2048;
						ptrSrc4 = ptrSrc3 + 2048;
						ptrSrc = ptrSrc4 + 2048;

						for(k = 0; k < 32; k++)
						{
							pDstU1 = pDstU0 + offset + k * nHalfWidth;
							pDstV1 = pDstV0 + offset + k * nHalfWidth;
							pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
							pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;
							pDstU3 = pDstU0 + offset2 + k * nHalfWidth;
							pDstV3 = pDstV0 + offset2 + k * nHalfWidth;
							pDstU4 = pDstU0 + offset2 + 32 + k * nHalfWidth;
							pDstV4 = pDstV0 + offset2 + 32 + k * nHalfWidth;

							for(l = 0; l < 32; l++) // 64 / 2
							{
								// 1
								*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
								*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
								if(l < nLimitW)
								{
									// 2
									*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
									*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
								}
								else
								{
									ptrSrc2 += 2;
								}
							}
							if(k < nSubRemainH)
							{
								for(l = 0; l < 32; l++) // 64 / 2
								{
									// 3
									*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
									*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
									if(l < nLimitW)
									{
										// 4
										*(pDstU4 + l) = *ptrSrc4;ptrSrc4++;
										*(pDstV4 + l) = *ptrSrc4;ptrSrc4++;
									}
									else
									{
										ptrSrc4 += 2;
									}
								}
							}
							else
							{
								ptrSrc3 += 64;
								ptrSrc4 += 64;
							}
						}
					}
					else
					{
						int nLimitW = nRemainW >> 1;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc3 = ptrSrc2 + 2048;
						ptrSrc4 = ptrSrc3 + 2048;
						ptrSrc = ptrSrc4 + 2048;

						for(k = 0; k < 32; k++)
						{
							pDstU1 = pDstU0 + offset + k * nHalfWidth;
							pDstV1 = pDstV0 + offset + k * nHalfWidth;
							pDstU3 = pDstU0 + offset2 + k * nHalfWidth;
							pDstV3 = pDstV0 + offset2 + k * nHalfWidth;

							for(l = 0; l < nLimitW; l++) // nRemainW / 2
							{
								// 1
								*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
								*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
							}
							ptrSrc1 += 64 - nRemainW;
							if(k < nSubRemainH)
							{
								for(l = 0; l < nLimitW; l++) // nRemainW / 2
								{
									// 3
									*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
									*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
								}
								ptrSrc3 += 64 - nRemainW;
							}
							else
							{
								ptrSrc3 += 64;
							}
						}
					}
				}
			}
		}
		else
		{
			int nSubRemainH = nRemainH;
			for(j = 0; j < tile_wcount; j++)
			{
				offset = tile_hcount * (srcWidth << 5) + (j << 7);
				offset2 = offset + (srcWidth << 4);

				ptrSrc1 = ptrSrc;
				ptrSrc2 = ptrSrc1 + 2048;
				ptrSrc3 = ptrSrc2 + 2048;
				ptrSrc4 = ptrSrc3 + 2048;
				ptrSrc = ptrSrc4 + 2048;

				for(k = 0; k < nSubRemainH; k++)
				{
					pDstU1 = pDstU0 + offset + k * nHalfWidth;
					pDstV1 = pDstV0 + offset + k * nHalfWidth;
					pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
					pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;
					pDstU3 = pDstU0 + offset + 64 + k * nHalfWidth;
					pDstV3 = pDstV0 + offset + 64 + k * nHalfWidth;
					pDstU4 = pDstU0 + offset + 96 + k * nHalfWidth;
					pDstV4 = pDstV0 + offset + 96 + k * nHalfWidth;

					for(l = 0; l < 32; l++) // 64 / 2
					{
						// 1
						*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
						*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
						// 2
						*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
						*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
						// 3
						*(pDstU3 + l) = *ptrSrc3;ptrSrc3++;
						*(pDstV3 + l) = *ptrSrc3;ptrSrc3++;
						// 4
						*(pDstU4 + l) = *ptrSrc4;ptrSrc4++;
						*(pDstV4 + l) = *ptrSrc4;ptrSrc4++;
					}
				}
			}

			if(nRemainW > 0)
			{
				offset = tile_hcount * (srcWidth << 5) + (tile_wcount << 7);
				offset2 = offset + (srcWidth << 4);
				if(nRemainW > 128)
				{
					ptrSrc1 = ptrSrc;
					ptrSrc2 = ptrSrc1 + 2048;
					ptrSrc = ptrSrc2 + 2048;

					for(k = 0; k < nSubRemainH; k++)
					{
						pDstU1 = pDstU0 + offset + k * nHalfWidth;
						pDstV1 = pDstV0 + offset + k * nHalfWidth;
						pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
						pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;

						for(l = 0; l < 32; l++) // 64 / 2
						{
							// 1
							*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
							*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
							// 2
							*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
							*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
						}
					}

					int nSubRemainW = nRemainW - 128;
					if(nSubRemainW > 64)
					{
						nSubRemainW -= 64;
						int nLimitW = nSubRemainW >> 1;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc = ptrSrc2 + 2048;

						for(k = 0; k < nSubRemainH; k++)
						{
							pDstU1 = pDstU0 + offset + 64 + k * nHalfWidth;
							pDstV1 = pDstV0 + offset + 64 + k * nHalfWidth;
							pDstU2 = pDstU0 + offset + 96 + k * nHalfWidth;
							pDstV2 = pDstV0 + offset + 96 + k * nHalfWidth;

							for(l = 0; l < 32; l++) // 64 / 2
							{
								// 1
								*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
								*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
								if(l < nLimitW)
								{
									// 2
									*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
									*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
								}
								else
								{
									ptrSrc2 += 2;
								}
							}
						}
					}
					else
					{
						int nLimitW = nSubRemainW >> 1;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc = ptrSrc2 + 2048;

						for(k = 0; k < nSubRemainH; k++)
						{
							pDstU1 = pDstU0 + offset + 64 + k * nHalfWidth;
							pDstV1 = pDstV0 + offset + 64 + k * nHalfWidth;

							for(l = 0; l < nLimitW; l++) // nSubRemainW / 2
							{
								// 1
								*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
								*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
							}
							ptrSrc1 += 64 - nSubRemainW;
						}
					}
				}
				else // if(nRemainW <= 128)
				{
					if(nRemainW > 64)
					{
						int nSubRemainW = nRemainW - 64;

						int nLimitW = nSubRemainW >> 1;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc = ptrSrc2 + 2048;

						for(k = 0; k < nSubRemainH; k++)
						{
							pDstU1 = pDstU0 + offset + k * nHalfWidth;
							pDstV1 = pDstV0 + offset + k * nHalfWidth;
							pDstU2 = pDstU0 + offset + 32 + k * nHalfWidth;
							pDstV2 = pDstV0 + offset + 32 + k * nHalfWidth;

							for(l = 0; l < 32; l++) // 64 / 2
							{
								// 1
								*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
								*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
								if(l < nLimitW)
								{
									// 2
									*(pDstU2 + l) = *ptrSrc2;ptrSrc2++;
									*(pDstV2 + l) = *ptrSrc2;ptrSrc2++;
								}
								else
								{
									ptrSrc2 += 2;
								}
							}
						}
					}
					else
					{
						int nLimitW = nRemainW >> 1;

						ptrSrc1 = ptrSrc;
						ptrSrc2 = ptrSrc1 + 2048;
						ptrSrc = ptrSrc2 + 2048;

						for(k = 0; k < nSubRemainH; k++)
						{
							pDstU1 = pDstU0 + offset + k * nHalfWidth;
							pDstV1 = pDstV0 + offset + k * nHalfWidth;
							for(l = 0; l < nLimitW; l++) // nRemainW / 2
							{
								// 1
								*(pDstU1 + l) = *ptrSrc1;ptrSrc1++;
								*(pDstV1 + l) = *ptrSrc1;ptrSrc1++;
							}
							ptrSrc1 += 64 - nRemainW;
						}
					}
					ptrSrc += 4096;
				}
			}
		}
	}
}


// yuv420 is INOUT buffer
// tmp is temporary buffer, it's size must big than srcWidth*srcHeight/2
void yuv420_to_yuv420sp(char* yuv420, int srcWidth, int srcHeight, char* tmp)
{
	int size = srcWidth * srcHeight;
	int pos = size >> 1; //  size/2
	int stride = size >> 2; //  size/4

	// backup yuv color info to tmp
	char* ptr = yuv420 + size;
	memcpy(tmp, ptr, pos);

	int i, j;

	char* pSrc1 = tmp;
	char* pSrc2 = tmp + stride;

	for (i=0; i<stride; i++)
	{
		*(ptr + i * 2) = *(pSrc1 + i);
		*(ptr + i * 2 + 1) = *(pSrc2 + i);
	}
}

void write2File(void* buff, int size, int index)
{
	//char* name = "/storage/sdcard0/bmp";
	char name[255] = {0,};
	sprintf(name, "/storage/sdcard0/yuv%d", index);

	FILE * fp = fopen(name, "wb");

	if (fp){
		fwrite(buff, 1, size, fp);
		fclose(fp);
	}

}

// convert RGB565 to RGBA8888
int rgb565_to_rgb8888(char* src, char* dest, int width, int height)
{
	int idx;
	int idy;
	short* source = (short*)src;

	for (idy = 0; idy < height; idy++) {
		for (idx = 0; idx < width; idx++) {
			short clr = *source++;
			*dest++ = GET_B(clr);
			*dest++ = GET_G(clr);
			*dest++ = GET_R(clr);
			*dest++ = 255;
		}
	}

	return 1;
}

// convert BGR565 to RGBA8888
int bgr565_to_rgb8888(char* src, char* dest, int width, int height)
{
	int idx;
	int idy;
	short* source = (short*)src;

	for (idy = 0; idy < height; idy++) {
		for (idx = 0; idx < width; idx++) {
			short clr = *source++;
			*dest++ = GET_R(clr);
			*dest++ = GET_G(clr);
			*dest++ = GET_B(clr);
			*dest++ = 255;
		}
	}

	return 1;
}

// convert BGRA8888 to RGBA8888
int bgr8888_to_rgb8888(char* src, char* dest, int width, int height)
{
	int idx;
	int idy;
	char r, g, b, a;
	for (idy = 0; idy < height; idy++) {
		for (idx = 0; idx < width; idx++) {
			r = *src++;
			g = *src++;
			b = *src++;
			a = *src++;
			*dest++ = b;
			*dest++ = g;
			*dest++ = r;
			*dest++ = a;
		}
	}

	return 1;
}
