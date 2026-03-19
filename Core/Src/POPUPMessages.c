
#include "J1939.h"
#include "POPUPMessages.h"
#include "TML_14229_UDSH.h"
#include "extEEPROM.h"


#define MSG_ROTATE_TIME			4000/2	/* 1ms * 4000 = 4sec*/

#define FIVE_SECONDS 5000U/2u
#define FOUR_SECONDS 4000U/2u
#define SIX_SECONDS 6000U/2u

// Defining DTC_SPNs as per Document "BSVI_IC_LCD-Text Message -V1.1_09112019.xls "
#define DTC_SPN132 132U
#define DTC_SPN106 106U
#define DTC_SPN168 168U
#define DTC_SPN167 167U
#define DTC_SPN158 158U
#define DTC_SPN110 110U
#define DTC_SPN2814 2814U
#define DTC_SPN7760 7760U
#define DTC_SPN2802 2802U
#define DTC_SPN7443 7443U
#define DTC_SPN2791 2791U
#define DTC_SPN5928 5928U
#define DTC_SPN190 	190U
#define DTC_SPN636 	636U
#define DTC_SPN637 	637U
#define DTC_SPN97 	97U
#define DTC_SPN5549 5549U
#define DTC_SPN5314 5314U
#define DTC_SPN651 	651U
#define DTC_SPN652 	652U
#define DTC_SPN653 	653U
#define DTC_SPN654 	654U
#define DTC_SPN156 	156U
#define DTC_SPN5358 5358U
#define DTC_SPN5359 5359U
#define DTC_SPN5360 5360U
#define DTC_SPN5361 5361U
#define DTC_SPN28 	28U
#define DTC_SPN7026 7026U
#define DTC_SPN898 	898U
#define DTC_SPN2432 2432U
#define DTC_SPN516098 516098U
#define DTC_SPN516099 516099U
#define DTC_SPN516100 516100U
#define DTC_SPN516101 516101U
#define DTC_SPN516105 516105U
#define DTC_SPN516106 516106U
#define DTC_SPN516107 516107U
#define DTC_SPN516108 516108U
#define DTC_SPN516110 516110U
#define DTC_SPN516111 516111U
#define DTC_SPN516113 516113U
#define DTC_SPN516114 516114U
#define DTC_SPN516115 516115U
#define DTC_SPN516116 516116U
#define DTC_SPN516118 516118U
#define DTC_SPN516119 516119U
#define DTC_SPN516120 516120U
#define DTC_SPN516121 516121U
#define DTC_SPN516122 516122U
#define DTC_SPN516123 516123U
#define DTC_SPN516124 516124U
#define DTC_SPN516125 516125U
#define DTC_SPN516126 516126U
#define DTC_SPN516127 516127U
#define DTC_SPN516128 516128U
#define DTC_SPN516129 516129U
#define DTC_SPN516130 516130U
#define DTC_SPN516131 516131U
#define DTC_SPN516132 516132U
#define DTC_SPN516133 516133U
#define DTC_SPN516134 516134U
#define DTC_SPN516135 516135U
#define DTC_SPN516136 516136U
#define DTC_SPN516137 516137U
#define DTC_SPN516138 516138U
#define DTC_SPN516139 516139U
#define DTC_SPN516140 516140U
#define DTC_SPN516141 516141U
#define DTC_SPN516142 516142U
#define DTC_SPN516143 516143U
#define DTC_SPN516144 516144U
#define DTC_SPN516145 516145U
#define DTC_SPN516146 516146U
#define DTC_SPN516147 516147U
#define DTC_SPN516148 516148U
#define DTC_SPN516149 	516149U
#define DTC_SPN516150 	516150U
#define DTC_SPN516151 	516151U
#define DTC_SPN516152 	516152U
#define DTC_SPN516153 	516153U
#define DTC_SPN516154 	516154U
#define DTC_SPN516155 	516155U
#define DTC_SPN516156 	516156U
#define DTC_SPN516157 	516157U
#define DTC_SPN516158 	516158U
#define DTC_SPN516159 	516159U
#define DTC_SPN516160 	516160U
#define DTC_SPN516161 	516161U
#define DTC_SPN516162 	516162U
#define DTC_SPN516163 	516163U
#define DTC_SPN516164 	516164U
#define DTC_SPN516165 	516165U
#define DTC_SPN516166 	516166U
#define DTC_SPN516167 	516167U
#define DTC_SPN516168 	516168U
#define DTC_SPN516169 	516169U
#define DTC_SPN516170 	516170U
#define DTC_SPN516171 	516171U
#define DTC_SPN516172 	516172U
#define DTC_SPN516173 	516173U
#define DTC_SPN516174 	516174U
#define DTC_SPN516175 	516175U
#define DTC_SPN516176 	516176U
#define DTC_SPN516177 	516177U
#define DTC_SPN516178 	516178U
#define DTC_SPN516179 	516179U
#define DTC_SPN516180 	516180U
#define DTC_SPN516181 	516181U
#define DTC_SPN100 		100U
#define DTC_SPN157 		157U
#define DTC_SPN633 		633U
#define DTC_SPN91 		91U
#define DTC_SPN29 		29U
#define DTC_SPN974 		974U
#define DTC_SPN975 		975U
#define DTC_SPN3509 	3509U
#define DTC_SPN3510 	3510U
#define DTC_SPN6385 	6385U
#define DTC_SPN677 		677U
#define DTC_SPN5838 	5838U
#define DTC_SPN1136 	1136U

//for 74hp TATA Engine

#define DTC_SPN168 168U
#define DTC_SPN110 110U
#define DTC_SPN5965 5965U
#define DTC_SPN5838 5838U
#define DTC_SPN2791 2791U
#define DTC_SPN5764 5764U
#define DTC_SPN5928 5928U
#define DTC_SPN3058 3058U
#define DTC_SPN27   27U
#define DTC_SPN5419 5419U
#define DTC_SPN5783 5783U
#define DTC_SPN3464 3464U
#define DTC_SPN3465 3465U
#define DTC_SPN51 	51U
#define DTC_SPN6650 6650U
#define DTC_SPN2812	2812U
#define DTC_SPN516170 516170U
#define DTC_SPN636 636U
#define DTC_SPN190 	190U
#define DTC_SPN97 	97U
#define DTC_SPN516264 516264U
#define DTC_SPN651 	651U
#define DTC_SPN653 	653U
#define DTC_SPN654 654U
#define DTC_SPN652 652U
#define DTC_SPN2797 2797U
#define DTC_SPN2798 2798U
#define DTC_SPN7026 7026U
#define DTC_SPN7027 7027U
#define DTC_SPN7029 7029U
#define DTC_SPN516171 516171U
#define DTC_SPN516172 516172U
#define DTC_SPN516173 516173U
#define DTC_SPN91     91U
#define DTC_SPN516280 516280U
#define DTC_SPN516281 516281U
#define DTC_SPN8322 8322U
#define DTC_SPN516276 516276U
#define DTC_SPN516175 516175U
#define DTC_SPN516176 516176U
#define DTC_SPN516197 516197U
#define DTC_SPN516097 516097U
#define DTC_SPN516177 516177U
#define DTC_SPN516178 516178U
#define DTC_SPN516179 516179U
#define DTC_SPN516180 516180U
#define DTC_SPN516181 516181U
#define DTC_SPN516182 516182U
#define DTC_SPN516183 516183U
#define DTC_SPN516184 516184U
#define DTC_SPN516185 516185U
#define DTC_SPN516186 516186U
#define DTC_SPN516187 516187U
#define DTC_SPN516196 516196U
#define DTC_SPN516096 516196U
//#define DTC_SPN516098 516198U
//#define DTC_SPN516099 516199U
#define DTC_SPN516100 516100U
#define DTC_SPN516101 516101U
#define DTC_SPN516102 516102U
#define DTC_SPN516103 516103U
#define DTC_SPN516104 516104U
#define DTC_SPN516105 516105U
#define DTC_SPN516106 516106U
#define DTC_SPN516107 516107U
#define DTC_SPN516167 516167U
#define DTC_SPN516108 516108U
#define DTC_SPN516109 516109U
#define DTC_SPN516110 516110U
#define DTC_SPN516111 516111U
#define DTC_SPN516112 516112U
#define DTC_SPN516113 516113U
#define DTC_SPN516117 516117U
#define DTC_SPN516114 516114U
#define DTC_SPN516115 516115U
#define DTC_SPN516116 516116U
#define DTC_SPN516157 516157U
#define DTC_SPN516118 516118U
#define DTC_SPN516168 516168U
#define DTC_SPN516119 516119U
#define DTC_SPN516120 	516120U
#define DTC_SPN516121 	516121U
#define DTC_SPN516122 	516122U
#define DTC_SPN516123 	516123U
#define DTC_SPN516124 	516124U
#define DTC_SPN516125 	516125U
#define DTC_SPN516126 	516126U
#define DTC_SPN516127 	516127U
#define DTC_SPN516128 	516128U
#define DTC_SPN516129 	516129U
#define DTC_SPN516130 	516130U
#define DTC_SPN516131 	516131U
#define DTC_SPN516132   516132U
#define DTC_SPN516162   516162U
#define DTC_SPN516169   516169U
#define DTC_SPN516166   516166U
#define DTC_SPN516152 	516152U
#define DTC_SPN516133 	516133U
#define DTC_SPN516134 	516134U
#define DTC_SPN516135 	516135U
#define DTC_SPN516136 	516136U
#define DTC_SPN516137 	516137U
#define DTC_SPN516138 	516138U
#define DTC_SPN516139 	516139U
#define DTC_SPN516140 	516140U
#define DTC_SPN516141 	516141U
#define DTC_SPN516142 	516142U
#define DTC_SPN516143 	516143U
#define DTC_SPN516144 	516144U
#define DTC_SPN516145 	516145U
#define DTC_SPN516146 	516146U
#define DTC_SPN516147 	516147U
#define DTC_SPN516148 	516148U
#define DTC_SPN516149 	516149U
#define DTC_SPN516150 	516150U
#define DTC_SPN516151 	516151U
#define DTC_SPN516159 	516159U
#define DTC_SPN516153 	516153U
#define DTC_SPN516154 	516154U
#define DTC_SPN516155 	516155U
#define DTC_SPN516165 	516165U
#define DTC_SPN100 		100U
#define DTC_SPN108 		108U
#define DTC_SPN18 		18U
#define DTC_SPN157		157U
#define DTC_SPN4765 	4765U
#define DTC_SPN3242 	3242U
#define DTC_SPN3251 	3251U
#define DTC_SPN3720 	3720U
#define DTC_SPN3719 	3719U
#define DTC_SPN4795 	4795U
#define DTC_SPN3696     3696U
#define DTC_SPN5397 	5397U
#define DTC_SPN5395 	5395U
#define DTC_SPN516266 	516266U
#define DTC_SPN516267 	516267U
#define DTC_SPN516268 	516268U
#define DTC_SPN516269 	516269U
#define DTC_SPN516270 	516270U
#define DTC_SPN5571 	5571U
#define DTC_SPN516271 	516271U
#define DTC_SPN516156 	516156U
#define DTC_SPN516257 	516257U
#define DTC_SPN516259 	516259U
//#define DTC_SPN516160 	516260U
//#define DTC_SPN516161 	516261U
#define DTC_SPN516261 	516261U
#define DTC_SPN516262 	516262U
#define DTC_SPN516265 	516265U
#define DTC_SPN516266 	516266U
#define DTC_SPN516267 	516267U
#define DTC_SPN516268 	516268U
#define DTC_SPN516269 	516269U
#define DTC_SPN29 	    29U
#define DTC_SPN3509 	3509U
#define DTC_SPN3510 	3510U
#define DTC_SPN677 	    677U
#define DTC_SPN6385 	6385U
#define DTC_SPN5826 	5826U
#define DTC_SPN105 	    105U
#define DTC_SPN3563 	3563U
#define DTC_SPN102 	    102U
#define DTC_SPN1624 	1624U
#define DTC_SPN604 	    604U

// Defining DTC_FMIs as per Document "BSVI_IC_LCD-Text Message -V1.1_09112019.xls "
// Defining DTC_FMIs as per Document "BSVI_IC_LCD-Text Message -V1.1_09112019.xls "
#define DTC_FMI12 12U
#define DTC_FMI13 13U
#define DTC_FMI2  2U
#define DTC_FMI16 16U
#define DTC_FMI18 18U
#define DTC_FMI19 19U
#define DTC_FMI3  3U
#define DTC_FMI4  4U
#define DTC_FMI31 31U
#define DTC_FMI5  5U
#define DTC_FMI0  0U
#define DTC_FMI9  9U
#define DTC_FMI22 22U
#define DTC_FMI6  6U
#define DTC_FMI17 17U
#define DTC_FMI14 14U
#define DTC_FMI1  1U
#define DTC_FMI8  8U
#define DTC_FMI15 15U
#define DTC_FMI7  7U
#define DTC_FMI11 11U
#define DTC_FMI10 10U


const DTCLookupTable DtcSpnNFmiTableKoel74Hp[eDtc74hpKoelTextMaxNumber] =  //storing data in ROM(Internal flash as its not changing)
   {
	{0   ,  0   , "        "},
	{28  ,  16  , "P0227-11"},
	{29  ,  3  ,  "P0220-12"},
	{29  ,  4  ,  "P0220-14"},
	{91  ,  3  ,  "P0120-12"},
	{91  ,  4  ,  "P0120-14"},
	{91  ,  2  ,  "P2135-0"},
	{97  ,  5  ,  "P2265-0"},
	{97  ,  2  ,  "P2269-0"},
	{100  ,  1  ,  "P0524-84"},
	{100  ,  3  ,  "P0523-12"},
	{100  ,  4  ,  "P0522-11"},
	{106  ,  3  ,  "P0100-85"},
	{106  ,  4  ,  "P0100-84"},
	{110  ,  16  ,  "P0115-85"},
	{110  ,  3  ,  "P0118-0"},
	{110  ,  4  ,  "P0117-0"},
	{132  ,  12  ,  "P2430-24"},
	{132  ,  1  ,  "P2430-23"},
	{132  ,  2  ,  "P2430-4"},
	{132  ,  16  ,  "P0100-24"},
	{132  ,  18  ,  "P0100-23"},
	{132  ,  19  ,  "P0100-87"},
	{132  ,  31  ,  "P0102-87"},
	{132  ,  3  ,  "P0102-85"},
	{132  ,  4  ,  "P0102-84"},
	{156  ,  14  ,  "P062D-F0"},
	{156  ,  15  ,  "P062E-F0"},
	{157  ,  13  ,  "P0089-85"},
	{157  ,  16  ,  "P0089-F0"},
	{157  ,  0  ,  "P0089-F1"},
	{157  ,  14  ,  "P0089-F2"},
	{157  ,  17  ,  "P1037-12"},
	{157  ,  31  ,  "P0089-F3"},
	{157  ,  3  ,  "P0193-0"},
	{157  ,  4  ,  "P0192-0"},
	{158  ,  3  ,  "P0560-A3"},
	{158  ,  4  ,  "P0560-A2"},
	{158  ,  11  ,  "P2530-1"},
	{167  ,  3  ,  "P0563-A3"},
	{167  ,  4  ,  "P0562-A2"},
	{168  ,  3  ,  "P0659-12"},
	{168  ,  4  ,  "P0658-11"},
	{190  ,  0  ,  "P0219-0"},
	{633  ,  3  ,  "P0087-0"},
	{633  ,  4  ,  "P0088-0"},
	{633  ,  11  ,  "P0088-F1"},
	{633  ,  5  ,  "P0088-64"},
	{633  ,  15  ,  "P1050-12"},
	{633  ,  16  ,  "P0251-F6"},
	{636  ,  8  ,  "P0340-1F"},
	{636  ,  5  ,  "P0340-31"},
	{637  ,  2  ,  "P0335-1F"},
	{637  ,  5  ,  "P0335-31"},
	{651  ,  5  ,  "P21CF-0"},
	{651  ,  3  ,  "P21DC-0"},
	{651  ,  4  ,  "P0262-0"},
	{652  ,  5  ,  "P21D0-0"},
	{652  ,  3  ,  "P21DF-0"},
	{652  ,  4  ,  "P0265-0"},
	{653  ,  5  ,  "P0203-13"},
	{653  ,  3  ,  "P0268-12"},
	{653  ,  4  ,  "P0267-11"},
	{654  ,  5  ,  "P0206-13"},
	{654  ,  3  ,  "P0277-12"},
	{654  ,  4  ,  "P0276-11"},
	{677  ,  5  ,  "P26E4-13"},
	{677  ,  16  ,  "P26E4-4B"},
	{677  ,  3  ,  "P26E4-12"},
	{677  ,  4  ,  "P26E4-11"},
	{898  ,  31  ,  "P0611-0"},
	{974  ,  3  ,  "P0225-17"},
	{974  ,  4  ,  "P0225-16"},
	{974  ,  2  ,  "P0225-64"},
	{975  ,  3  ,  "P0225-12"},
	{975  ,  4  ,  "P0225-11"},
	{1136  ,  3  ,  "P0666-22"},
	{1136  ,  4  ,  "P0666-21"},
	{2432  ,  2  ,  "P0611-F5"},
	{2432  ,  3  ,  "P0611-F8"},
	{2791  ,  22  ,  "P0403-13"},
	{2791  ,  5  ,  "P0487-0"},
	{2791  ,  6  ,  "P213A-A2"},
	{2791  ,  12  ,  "P0488-0"},
	{2791  ,  3  ,  "P2142-0"},
	{2791  ,  4  ,  "P2141-0"},
	{2791  ,  17  ,  "P213B-0"},
	{2791  ,  18  ,  "P0488-4B"},
	{2791  ,  14  ,  "P213B-A2"},
	{2791  ,  31  ,  "P0406-73"},
	{2791  ,  0  ,  "P0402-72"},
	{2791  ,  1  ,  "P0406-0"},
	{2791  ,  2  ,  "P0405-0"},
	{2802  ,  3  ,  "P0563-0"},
	{2802  ,  4  ,  "P0562-0"},
	{2814  ,  5  ,  "P065D-13"},
	{2814  ,  16  ,  "P065D-4B"},
	{2814  ,  4  ,  "P065D-15"},
	{2814  ,  3  ,  "P065D-14"},
	{3509  ,  3  ,  "P0641-0"},
	{3510  ,  3  ,  "P0651-0"},
	{5314  ,  15  ,  "P062B-A2"},
	{5358  ,  2  ,  "P268C-56"},
	{5359  ,  2  ,  "P268D-56"},
	{5360  ,  2  ,  "P268E-56"},
	{5361  ,  2  ,  "P268F-56"},
	{5549  ,  5  ,  "P0540-13"},
	{5549  ,  6  ,  "P0540-4B"},
	{5549  ,  3  ,  "P0540-12"},
	{5549  ,  4  ,  "P0540-11"},
	{5838  ,  2  ,  "P1068-0"},
	{5838  ,  3  ,  "P1069-0"},
	{5838  ,  1  ,  "P106A-0"},
	{5928  ,  3  ,  "P213D-0"},
	{5928  ,  4  ,  "P213C-0"},
	{6385  ,  3  ,  "P0615-12"},
	{6385  ,  4  ,  "P0615-11"},
	{7026  ,  5  ,  "P0251-13"},
	{7026  ,  3  ,  "P0254-F0"},
	{7026  ,  7  ,  "P0253-F0"},
	{7026  ,  4  ,  "P0254-F1"},
	{7026  ,  8  ,  "P0253-F1"},
	{7026  ,  11  ,  "P0251-64"},
	{7443  ,  5  ,  "P2536-9F"},
	{7760  ,  0  ,  "P0725-1C"},
	{7760  ,  13  ,  "P0725-1"},
	{7760  ,  2  ,  "P0725-1F"},
	{7760  ,  9  ,  "P0725-31"},
	{516098  ,  0  ,  "P060B-0"},
	{516099  ,  0  ,  "P060B-1C"},
	{516100  ,  0  ,  "P060B-53"},
	{516101  ,  0  ,  "P060D-0"},
	{516105  ,  0  ,  "P061C-0"},
	{516106  ,  0  ,  "P1001-62"},
	{516107  ,  0  ,  "P026B-0"},
	{516108  ,  0  ,  "P062B-0"},
	{516110  ,  0  ,  "P1002-62"},
	{516111  ,  0  ,  "P062B-4"},
	{516113  ,  0  ,  "P061B-4"},
	{516114  ,  0  ,  "P062B-78"},
	{516115  ,  0  ,  "P0190-64"},
	{516116  ,  0  ,  "P1082-0"},
	{516118  ,  0  ,  "P061B-0"},
	{516119  ,  0  ,  "P068A-0"},
	{516120  ,  0  ,  "P1084-0"},
	{516121  ,  0  ,  "P05CB-0"},
	{516122  ,  0  ,  "P1003-0"},
	{516123  ,  0  ,  "P1004-0"},
	{516124  ,  0  ,  "P1005-0"},
	{516125  ,  0  ,  "P1006-0"},
	{516126  ,  0  ,  "P1007-0"},
	{516127  ,  0  ,  "P1008-0"},
	{516128  ,  0  ,  "P1009-0"},
	{516129  ,  0  ,  "P1010-0"},
	{516130  ,  0  ,  "P1011-0"},
	{516131  ,  0  ,  "P1012-0"},
	{516132  ,  0  ,  "P1013-0"},
	{516133  ,  0  ,  "P1014-0"},
	{516134  ,  0  ,  "P1015-0"},
	{516135  ,  0  ,  "P1016-0"},
	{516136  ,  0  ,  "P1017-0"},
	{516137  ,  0  ,  "P1018-0"},
	{516138  ,  0  ,  "P1019-0"},
	{516139  ,  0  ,  "P1020-0"},
	{516140  ,  0  ,  "P1021-0"},
	{516141  ,  0  ,  "P1022-0"},
	{516142  ,  0  ,  "P1023-0"},
	{516143  ,  0  ,  "P1024-0"},
	{516144  ,  0  ,  "P1025-0"},
	{516145  ,  0  ,  "P1026-0"},
	{516146  ,  0  ,  "P1027-0"},
	{516147  ,  0  ,  "P1028-0"},
	{516148  ,  0  ,  "P1029-0"},
	{516149  ,  0  ,  "P1030-0"},
	{516150  ,  0  ,  "P1031-0"},
	{516151  ,  0  ,  "P1032-0"},
	{516152  ,  0  ,  "P1033-0"},
	{516153  ,  0  ,  "P1034-0"},
	{516154  ,  0  ,  "P1035-0"},
	{516155  ,  0  ,  "P1036-0"},
	{516156  ,  0  ,  "P1037-0"},
	{516157  ,  0  ,  "P1038-0"},
	{516158  ,  0  ,  "P1039-0"},
	{516159  ,  0  ,  "P1040-0"},
	{516160  ,  0  ,  "P1041-0"},
	{516161  ,  0  ,  "P1042-0"},
	{516162  ,  0  ,  "P1043-0"},
	{516163  ,  0  ,  "P1044-0"},
	{516164  ,  0  ,  "P1045-0"},
	{516165  ,  0  ,  "P1046-0"},
	{516166  ,  0  ,  "P1047-0"},
	{516167  ,  0  ,  "P1048-0"},
	{516168  ,  0  ,  "P1049-0"},
	{516169  ,  0  ,  "P1050-0"},
	{516170  ,  0  ,  "P1051-0"},
	{516171  ,  0  ,  "P1052-0"},
	{516172  ,  0  ,  "P1053-0"},
	{516173  ,  0  ,  "P1054-0"},
	{516174  ,  0  ,  "P1055-0"},
	{516175  ,  0  ,  "P1056-0"},
	{516176  ,  0  ,  "P1057-0"},
	{516177  ,  0  ,  "P1058-0"},
	{516178  ,  0  ,  "P060C-0"},
	{516179  ,  0  ,  "P060C-87"},
	{516180  ,  0  ,  "P060C-48"},
	{516181  ,  0  ,  "P060C-17"},


};

const DTCLookupTable DtcSpnNFmiTableTata74Hp[eDtc74hpTataTextMaxNumber] =  //storing data in ROM(Internal flash as its not changing)
   {
		{  0,   0 , "        "},
		{168 ,  0 , "P0560-A3"},
		{168 ,  1 , "P0560-A2"},
		{110 ,  9 , "P0115-22"},
		{110 ,  3 , "P0115-15"},
		{110 ,  4 , "P0115-11"},
		{5965 , 16 ,"P056D-A3"},
		{5965 , 18 ,"P056D-A2"},
		{5838 , 5 , "P0403-18"},
		{2791 , 5 , "P0403-13"},
		{5838 , 6 , "P0403-1D"},
		{5764 , 16 ,"P0403-4B"},
		{2791 , 3 , "P0403-F0"},
		{5928 , 3 , "P0403-F2"},
		{2791 , 4 , "P0403-F1"},
		{5928 , 4 , "P0403-F3"},
		{5838 , 12 ,"P0403-2B"},
		{3058 , 5 , "P0403-19"},
		{3058 , 4 , "P0403-A2"},
		{5838 , 18 ,"P0403-73"},
		{5838 , 16 ,"P0403-72"},
		{27 , 3 ,   "P0409-12"},
		{27 , 4 ,   "P0409-14"},
		{5419 , 5 , "P2100-13"},
		{5419 , 6 , "P2100-19"},
		{5783 , 0 , "P2100-4B"},
		{3464 , 3 , "P0638-12"},
		{3465 , 3 , "P0639-12"},
		{3464 , 4 , "P0638-11"},
		{3465 , 4 , "P0639-11"},
		{5419 , 11 ,"P2100-94"},
		{5419 , 31 ,"P2100-1"},
		{5419 , 4 , "P2100-16"},
		{51 , 3 ,   "P0120-12"},
		{51 , 4 ,   "P0120-14"},
		{6650 , 7 , "P2100-9"},
		{51 , 7 ,   "P2100-4"},
		{2812 , 31 ,"P0219-0"},
		{516170 , 12 , "P1008-0"},
		{636 , 2 ,  "P0340-2F"},
		{636 , 11 , "P0340-31"},
		{190 , 10 , "P0016-0"},
		{190 , 2 ,  "P0335-1F"},
		{190 , 11 , "P0335-31"},
		{97 , 3 ,   "P2264-15"},
		{97 , 4 ,   "P2264-11"},
		{97 , 31 ,  "P2269-0"},
		{516264 , 16 , "P062B-0"},
		{651 , 5 , "P0201-13"},
		{653 , 5 , "P0203-13"},
		{654 , 5 , "P0204-13"},
		{652 , 5 , "P0202-13"},
		{2797 , 6 , "P062D-0"},
		{2798 , 6 , "P062E-0"},
		{651 , 4 , "P0201-11"},
		{653 , 4 , "P0203-11"},
		{654 , 4 , "P0204-11"},
		{652 , 4 , "P0202-11"},
		{651 , 3 , "P0201-2B"},
		{653 , 3 , "P0203-2B"},
		{654 , 3 , "P0204-2B"},
		{652 , 3 , "P0202-2B"},
		{651 , 13 , "P0201-56"},
		{653 , 13 , "P0203-56"},
		{654 , 13 , "P0204-56"},
		{652 , 13 , "P0202-56"},
		{7026 , 5 , "P0001-13"},
		{7027 , 3 , "P0001-F0"},
		{7029 , 3 , "P0001-F2"},
		{7027 , 4 , "P0001-F1"},
		{7029 , 4 , "P0001-F3"},
		{7026 , 6 , "P0001-2B"},
		{516171 , 12 , "P060B-F0"},
		{516172 , 12 , "P060B-0"},
		{516173 , 12 , "P060B-F2"},
		{91 , 12 ,     "P060D-0"},
		{516280 , 31 , "P062B-F6"},
		{516281 , 31 , "P062B-F7"},
		{8322 , 2 ,    "P058A-0"},
		{516276 , 12 , "P061C-1"},
		{516175 , 12 , "P1014-0"},
		{516176 , 12 , "P1015-0"},
		{516197 , 31 , "P1013-0"},
		{516177 , 12 , "P1018-0"},
		{516178 , 12 , "P1019-0"},
		{516179 , 12 , "P1080-0"},
		{516180 , 12 , "P1081-0"},
		{516181 , 12 , "P101B-0"},
		{516182 , 12 , "P101C-0"},
		{516183 , 12 , "P1082-0"},
		{516184 , 12 , "P1083-0"},
		{516185 , 12 , "P061B-0"},
		{516186 , 12 , "P1084-0"},
		{516187 , 12 , "P1085-0"},
		{516096 , 12 , "P1644-1"},
		{516097 , 12 , "P1640-2"},
		{516098 , 12 , "P1641-3"},
		{516099 , 12 , "P1642-4"},
		{516100 , 12 , "P1643-5"},
		{516101 , 12 , "P1645-6"},
		{516102 , 12 , "P1646-7"},
		{516103 , 12 , "P1647-8"},
		{516104 , 12 , "P1648-9"},
		{516105 , 12 , "P1649-10"},
		{516106 , 12 , "P1654-11"},
		{516107 , 12 , "P1650-12"},
		{516108 , 12 , "P1651-13"},
		{516109 , 12 , "P1652-14"},
		{516110 , 12 , "P1653-15"},
		{516111 , 12 , "P1655-16"},
		{516112 , 12 , "P1656-17"},
		{516113 , 12 , "P1657-18"},
		{516114 , 12 , "P1658-19"},
		{516115 , 12 , "P1659-20"},
		{516116 , 12 , "P1660-21"},
		{516117 , 12 , "P1661-22"},
		{516118 , 12 , "P1662-23"},
		{516119 , 12 , "P1663-24"},
		{516120 , 12 , "P1664-25"},
		{516121 , 12 , "P1665-26"},
		{516122 , 12 , "P1666-27"},
		{516123 , 12 , "P1667-28"},
		{516124 , 12 , "P1674-29"},
		{516125 , 12 , "P1668-30"},
		{516126 , 12 , "P1669-31"},
		{516127 , 12 , "P1670-32"},
		{516128 , 12 , "P1671-33"},
		{516129 , 12 , "P1672-34"},
		{516130 , 12 , "P1673-35"},
		{516131 , 12 , "P1679-36"},
		{516132 , 12 , "P1675-37"},
		{516133 , 12 , "P1676-38"},
		{516134 , 12 , "P1677-39"},
		{516135 , 12 , "P1678-40"},
		{516136 , 12 , "P1680-41"},
		{516137 , 12 , "P1681-42"},
		{516138 , 12 , "P1682-43"},
		{516139 , 12 , "P1683-44"},
		{516140 , 12 , "P1684-45"},
		{516141 , 12 , "P1685-46"},
		{516142 , 12 , "P1686-47"},
		{516143 , 12 , "P1690-48"},
		{516144 , 12 , "P1687-49"},
		{516145 , 12 , "P1688-50"},
		{516146 , 12 , "P1689-51"},
		{516147 , 12 , "P1691-52"},
		{516148 , 12 , "P1692-53"},
		{516149 , 12 , "P1693-54"},
		{516150 , 12 , "P1694-55"},
		{516151 , 12 , "P1699-56"},
		{516152 , 12 , "P060C-F0"},
		{516153 , 12 , "P060C-FC"},
		{516154 , 12 , "P060C-F1"},
		{516155 , 12 , "P060C-A3"},
		{100 , 1 , "P0524-0"},
		{100 , 2 , "P0520-64"},
		{108 , 15 , "P2226-22"},
		{108 , 17 , "P2226-21"},
		{18 , 18 , "P016F-0"},
		{18 , 16 , "P016E-0"},
		{18 , 0 ,  "P0088-22"},
		{18 , 15 , "P0088-0"},
		{5395 , 2 ,"P0251-96"},
		{157 , 0 , "P0190-15"},
		{157 , 4 , "P0190-11"},
		{4765 , 2 , "P2031-64"},
		{4765 , 3 , "P2031-15"},
		{4765 , 4 , "P2031-11"},
		{3242 , 2 , "P242A-64"},
		{3242 , 3 , "P242A-15"},
		{3242 , 4 , "P242A-11"},
		{3251 , 3 , "P2452-15"},
		{3251 , 4 , "P2452-11"},
		{3251 , 7 , "P2452-95"},
		{3251 , 2 , "P2452-64"},
		{3720 , 16 ,"P242F-0"},
		{3719 , 7 , "P244B-0"},
		{4795 , 31 , "P226D-0"},
		{3251 , 16 , "P244B-97"},
		{3696 , 7 , "P25BA-9E"},
		{3719 , 0 , "P246C-0"},
		{3719 , 15 , "P2463-0"},
		{3719 , 16 , "P24A4-0"},
		{5397 , 31 , "P2459-0"},
		{516266 , 8 , "P018F-0"},
		{516267 , 2 , "P000F-68"},
		{516268 , 2 , "P000F-9A"},
		{516269 , 2 , "P000F-0"},
		{516270 , 2 , "P009B-94"},
		{5571 , 7 , "P000F-22"},
		{516271 , 2 , "P018F-7"},
		{516156 , 12 , "P1971-0"},
		{516157 , 12 , "P1972-0"},
		{516159 , 12 , "P1632-0"},
		{516160 , 12 , "P1974-42"},
		{516161 , 12 , "P1975-42"},
		{516162 , 12 , "P1976-42"},
		{516165 , 12 , "P1633-42"},
		{516166 , 12 , "P1634-0"},
		{516167 , 12 , "P1979-49"},
		{516168 , 12 , "P1980-49"},
		{516169 , 12 , "P1981-49"},
		{91 , 3 , "P2120-15"},
		{29 , 3 , "P2125-15"},
		{91 , 4 , "P2120-11"},
		{29 , 4 , "P2125-11"},
		{91 , 2 , "P2138-0"},
		{3509 , 12 , "P0641-0"},
		{3510 , 12 , "P0651-0"},
		{677 , 3 , "P0615-F0"},
		{677 , 4 , "P0615-F1"},
		{677 , 5 , "P0615-13"},
		{677 , 11 , "P0615-4B"},
		{6385 , 3 , "P0615-F2"},
		{6385 , 4 , "P0615-F3"},
		{5826 , 12 , "P2BAB-93"},
		{5826 , 7 , "P2BAB-94"},
		{5826 , 14 , "P2BAB-0"},
		{105 , 3 , "P00E8-15"},
		{105 , 4 , "P00E8-11"},
		{3563 , 3 , "P0105-15"},
		{102 , 3 , "P0105-11"},
		{3563 , 16 , "P0105-24"},
		{3563 , 18 , "P0105-23"},
		{1624 , 4 , "P0500-11"},
		{604 , 9 , "P081D-64"},
};

const DTCLookupTable DtcSpnNFmiTableMandM74Hp[eDtc74hpMandMTextMaxNumber] =  //storing data in ROM(Internal flash as its not changing)
   {
		   {  0,   0 ,   "        "},
		   {29  ,  3  ,  "P223-0"},
		   {29  ,  4  ,  "P222-0"},
		   {51  ,  3  ,  "P2104-0"},
		   {51  ,  4  ,  "P2105-0"},
		   {51  ,  7  ,  "P2112-0"},
		   {51  ,  31  , "P2111-0"},
		   {51  ,  1  ,  "P2622-0"},
		   {51  ,  2  ,  "P2621-0"},
		   {91  ,  3  ,  "P123-0"},
		   {91  ,  4  ,  "P122-0"},
		   {91  ,  2  ,  "P60E-0"},
		   {97  ,  5  ,  "P2269-0"},
		   {97  ,  2  ,  "P2169-0"},
		   {100  ,  11  ,"P1FAF-0"},
		   {100  ,  1  , "P89B-0"},
		   {102  ,  3  , "P108-0"},
		   {102  ,  4  ,  "P1111-0"},
		   {102  ,  7  ,  "P69-0"},
		   {102  ,  7  ,  "P69-62"},
		   {105  ,  7  ,  "P306A-0"},
		   {105  ,  3  ,  "P113-0"},
		   {105  ,  4  ,  "P112-0"},
		   {108  ,  15  , "P2227-0"},
		   {108  ,  17  , "P2230-0"},
		   {110  ,  7  ,  "P306B-0"},
		   {110  ,  3  ,  "P118-0"},
		   {110  ,  4  ,  "P117-0"},
		   {156  ,  14  , "P2148-0"},
		   {156  ,  15  ,  "P2151-0"},
		   {157  ,  13  ,  "P18F-0"},
		   {157  ,  16  ,  "P95-0"},
		   {157  ,  21  ,  "P96-0"},
		   {157  ,  14  ,  "P194-0"},
		   {157  ,  31  ,  "P1110-0"},
		   {157  ,  3  ,  "P193-0"},
		   {157  ,  4  ,  "P192-0"},
		   {158  ,  3  ,  "P561-12"},
		   {158  ,  4  ,  "P560-11"},
		   {168  ,  3  ,  "P124E-0"},
		   {168  ,  4  ,  "P124F-0"},
		   {171  ,  3  ,  "P73-0"},
		   {171  ,  4  ,  "P72-0"},
		   {174  ,  3  ,  "P183-0"},
		   {174  ,  4  ,  "P182-0"},
		   {190  ,  11  , "P219-0"},
		   {633  ,  3  ,  "P87-0"},
		   {633  ,  4  ,  "P88-0"},
		   {633  ,  5  ,  "P89-0"},
		   {633  ,  14  , "P89C-0"},
		   {636  ,  2  ,  "P344-2F"},
		   {636  ,  5  ,  "P340-0"},
		   {636  ,  31  , "P341-0"},
		   {637  ,  2  ,  "P336-0"},
		   {637  ,  5  ,  "P335-0"},
		   {651  ,  5  ,  "P201-0"},
		   {651  ,  3  ,  "P1201-0"},
		   {651  ,  4  ,  "P262-0"},
		   {652  ,  5  ,  "P205-0"},
		   {652  ,  3  ,  "P1205-0"},
		   {652  ,  4  ,  "P274-0"},
		   {653  ,  5  ,  "P204-0"},
		   {653  ,  3  ,  "P1204-0"},
		   {653  ,  4  ,  "P271-0"},
		   {654  ,  5  ,  "P202-0"},
		   {654  ,  3  ,  "P1202-0"},
		   {654  ,  4  ,  "P265-0"},
		   {676  ,  2  ,  "P380-13"},
		   {676  ,  12  , "P380-4B"},
		   {676  ,  3  ,  "P384-0"},
		   {676  ,  4  ,  "P383-0"},
		   {898  ,  31  ,  "P62B-0"},
		   {1387  ,  7  ,  "P20A0-0"},
		   {1761  ,  15  ,  "P203A-0"},
		   {1761  ,  17  ,  "P20F4-0"},
		   {1761  ,  17  ,  "P2BA7-0"},
		   {1761  ,  18  ,  "P202F-0"},
		   {1761  ,  16  ,  "P203F-0"},
		   {2432  ,  2  ,   "P612-0"},
		   {2432  ,  3  ,   "P611-0"},
		   {2791  ,  13  ,  "P488-1D"},
		   {2791  ,  20  ,  "P49D-73"},
		   {2791  ,  21  ,  "P49D-72"},
		   {2791  ,  15  ,  "P404-0"},
		   {2791  ,  16  ,  "P1404-0"},
		   {2791  ,  5  ,   "P1403-0"},
		   {2791  ,  6  ,   "P408-0"},
		   {2791  ,  12  ,  "P488-4B"},
		   {2791  ,  3  ,   "P490-0"},
		   {2791  ,  4  ,   "P1489-0"},
		   {2791  ,  17  ,  "P409-0"},
		   {2791  ,  18  ,  "P488-0"},
		   {2791  ,  14  ,  "P403-0"},
		   {2791  ,  7  ,   "P42F-0"},
		   {2791  ,  31  ,  "P42E-0"},
		   {2791  ,  1  ,   "P1406-0"},
		   {2791  ,  2  ,   "P1405-0"},
		   {2802  ,  3  ,   "P563-0"},
		   {2802  ,  4  ,   "P562-0"},
		   {3031  ,  14  ,  "P5F8-0"},
		   {3031  ,  20  ,  "P24FF-0"},
		   {3031  ,  21  ,  "P205B-0"},
		   {3031  ,  19  ,  "P2701-0"},
		   {3031  ,  16  ,  "P24FE-0"},
		   {3031  ,  18  ,  "P225B-0"},
		   {3031  ,  3  ,   "P205A-0"},
		   {3031  ,  4  ,   "P205C-0"},
		   {3216  ,  12  ,  "P30C1-0"},
		   {3216  ,  7  ,   "P30C3-0"},
		   {3216  ,  20  ,  "P225E-0"},
		   {3216  ,  3  ,   "P2203-0"},
		   {3216  ,  4  ,  "P225D-0"},
		   {3216  ,  2  ,  "P2206-0"},
		   {3216  ,  6  ,  "P2202-0"},
		   {3216  ,  11  , "P229E-0"},
		   {3226  ,  12  , "P30C2-0"},
		   {3226  ,  7  ,  "P30C4-0"},
		   {3226  ,  20  ,  "P22FB-0"},
		   {3226  ,  3  ,  "P22A1-0"},
		   {3226  ,  4  ,  "P225F-0"},
		   {3226  ,  2  ,  "P2207-0"},
		   {3226  ,  6  ,  "P22A0-0"},
		   {3226  ,  11  , "P2200-0"},
		   {3241  ,  7  ,  "P306C-0"},
		   {3241  ,  2  ,  "P2080-A"},
		   {3241  ,  3  ,  "P546-0"},
		   {3241  ,  4  ,  "P545-0"},
		   {3245  ,  2  ,  "P247A-A"},
		   {3245  ,  3  ,  "P242D-0"},
		   {3245  ,  4  ,  "P242C-0"},
		   {3249  ,  2  ,  "P2084-A"},
		   {3249  ,  3  ,  "P2033-0"},
		   {3249  ,  4  ,  "P2022-0"},
		   {3251  ,  20  , "P2453-67"},
		   {3251  ,  2  ,  "P2453-84"},
		   {3251  ,  16  , "P30EB-0"},
		   {3251  ,  0  ,  "P30F0-0"},
		   {3251  ,  3  ,  "P2455-0"},
		   {3251  ,  4  ,  "P2454-0"},
		   {3270  ,  31  , "P30EA-0"},
		   {3361  ,  2  ,  "P2047-0"},
		   {3361  ,  3  ,  "P2049-0"},
		   {3361  ,  4  ,  "P2248-0"},
		   {3361  ,  6  ,  "P2046-0"},
		   {3364  ,  8  ,  "P216B-0"},
		   {3509  ,  3  ,  "P6B0-0"},
		   {3510  ,  3  ,  "P6B3-0"},
		   {3515  ,  19  , "P2065-0"},
		   {3515  ,  3  ,  "P206D-0"},
		   {3515  ,  4  ,  "P206C-0"},
		   {3516  ,  19  ,  "P206B-0"},
		   {3516  ,  31  ,  "P2062-0"},
		   {3516  ,  3  ,  "P2063-0"},
		   {3516  ,  4  ,  "P2064-0"},
		   {3517  ,  19  ,  "P202B-0"},
		   {3517  ,  16  ,  "P203B-0"},
		   {3517  ,  18  ,  "P213F-0"},
		   {3517  ,  3  ,   "P213A-0"},
		   {3517  ,  4  ,   "P203C-0"},
		   {3695  ,  19  ,  "P202F-8F"},
		   {3695  ,  11  ,  "P202F-62"},
		   {3695  ,  2  ,   "P260E-72"},
		   {3695  ,  12  ,  "P260E-98"},
		   {3695  ,  3  ,   "P260E-17"},
		   {3695  ,  4  ,   "P260E-16"},
		   {3695  ,  31  ,  "P202F-24"},
		   {3696  ,  31  ,  "P202F-2A"},
		   {3696  ,  2  ,   "P260E-13"},
		   {3696  ,  12  ,  "P260E-4B"},
		   {3696  ,  3  ,   "P260E-12"},
		   {3696  ,  4  ,   "P260E-11"},
		   {4094  ,  16  ,  "P2BA4-0"},
		   {4094  ,  0  ,   "P2BA5-0"},
		   {4094  ,  15  ,  "P26E4-0"},
		   {4095  ,  16  ,  "P2BA8-0"},
		   {4095  ,  0  ,   "P1BA7-0"},
		   {4095  ,  15  ,  "P26E3-0"},
		   {4225  ,  16  ,  "P2BAE-0"},
		   {4225  ,  0  ,   "P3BA4-0"},
		   {4225  ,  15  ,  "P26E5-0"},
		   {4339  ,  2  ,   "P204E-0"},
		   {4358  ,  21  ,  "P204F-0"},
		   {4358  ,  1  ,   "P20E6-0"},
		   {4358  ,  31  ,  "P20E7-0"},
		   {4358  ,  2  ,  "P22E8-0"},
		   {4358  ,  3  ,  "P204D-0"},
		   {4358  ,  4  ,  "P204C-0"},
		   {4364  ,  7  ,  "P202D-0"},
		   {4375  ,  19  ,  "P208B-0"},
		   {4375  ,  10  ,  "P209B-0"},
		   {4375  ,  31  ,  "P207B-0"},
		   {4375  ,  2  ,   "P208A-0"},
		   {4375  ,  12  ,  "P218B-0"},
		   {4375  ,  3  ,   "P208D-0"},
		   {4375  ,  4  ,  "P208C-0"},
		   {4376  ,  7  ,  "P20E8-0"},
		   {4376  ,  31  , "P20A1-0"},
		   {4376  ,  2  ,  "P21A0-0"},
		   {4376  ,  12  , "P21A1-0"},
		   {4376  ,  3  ,  "P20A3-0"},
		   {4376  ,  4  ,  "P20A2-0"},
		   {4384  ,  31  , "P20E9-0"},
		   {4384  ,  1  ,  "P21E8-0"},
		   {4389  ,  2  ,  "P218E-0"},
		   {4389  ,  31  ,  "P2048-0"},
		   {4781  ,  15  ,  "P2463-0"},
		   {4781  ,  16  ,  "P2463-92"},
		   {4795  ,  31  ,  "P30EC-0"},
		   {5314  ,  15  ,  "P62B-0"},
		   {5358  ,  2  ,  "P268C-0"},
		   {5359  ,  2  ,  "P268D-0"},
		   {5360  ,  2  ,  "P268E-0"},
		   {5361  ,  2  ,  "P268F-0"},
		   {5419  ,  20  ,  "P311B-0"},
		   {5419  ,  21  ,  "P311C-0"},
		   {5419  ,  15  ,  "P2173-0"},
		   {5419  ,  16  ,  "P2175-0"},
		   {5419  ,  5  ,  "P2100-0"},
		   {5419  ,  6  ,  "P2110-0"},
		   {5419  ,  12  , "P2118-4B"},
		   {5419  ,  3  ,  "P2103-0"},
		   {5419  ,  4  ,  "P2102-0"},
		   {5419  ,  17  ,  "P2113-0"},
		   {5419  ,  18  ,  "P2118-19"},
		   {5419  ,  14  ,  "P2118-16"},
		   {5549  ,  2  ,   "P543-0"},
		   {5549  ,  12  ,  "P607-0"},
		   {5549  ,  3  ,   "P542-0"},
		   {5549  ,  4  ,   "P541-0"},
		   {5838  ,  16  ,  "P2BAC-0"},
		   {5838  ,  0  ,   "P2BA3-0"},
		   {5838  ,  15  ,  "P26E2-0"},
		   {5928  ,  3  ,   "P45D-0"},
		   {5928  ,  4  ,   "P45C-0"},
		   {6875  ,  31  ,  "P2038-0"},
		   {6875  ,  1  ,   "P2039-0"},
		   {7026  ,  5  ,   "P251-0"},
		   {7026  ,  3  ,   "P252-0"},
		   {7026  ,  7  ,   "P254-0"},
		   {7026  ,  4  ,  "P253-0"},
		   {7026  ,  8  ,  "P255-0"},
		   {7026  ,  6  ,  "P256-0"},
		   {516098  ,  11  ,  "P162B-0"},
		   {516099  ,  11  ,  "P60B-0"},
		   {516100  ,  11  ,  "P63B-0"},
		   {516101  ,  12  ,  "P60D-0"},
		   {516101  ,  11  ,  "P59A-0"},
		   {516101  ,  0  ,  "P25F8-0"},
		   {516102  ,  0  ,  "P5F6-0"},
		   {516103  ,  11  ,  "P59B-0"},
		   {516103  ,  0  ,   "P5F5-0"},
		   {516104  ,  11  ,  "P58A-0"},
		   {516104  ,  0  ,  "P20FF-0"},
		   {516105  ,  11  ,  "P62C-0"},
		   {516105  ,  0  ,   "P20F1-0"},
		   {516106  ,  11  ,  "P1014-0"},
		   {516106  ,  0  ,   "P20F2-0"},
		   {516107  ,  11  ,  "P1015-0"},
		   {516107  ,  0  ,   "P20F3-0"},
		   {516108  ,  11  ,  "P1013-0"},
		   {516110  ,  11  ,  "P1018-0"},
		   {516111  ,  11  ,  "P1019-0"},
		   {516112  ,  11  ,  "P1080-0"},
		   {516113  ,  11  ,  "P1081-0"},
		   {516114  ,  11  ,  "P101B-0"},
		   {516115  ,  3  ,   "P26E9-12"},
		   {516115  ,  4  ,   "P26E8-11"},
		   {516115  ,  11  ,  "P101C-0"},
		   {516116  ,  11  ,  "P1082-0"},
		   {516117  ,  11  ,  "P1083-0"},
		   {516118  ,  11  ,  "P61B-0"},
		   {516119  ,  11  ,  "P61C-0"},
		   {516120  ,  11  ,  "P1084-0"},
		   {516121  ,  11  ,  "P1085-0"},
		   {516122  ,  14  ,  "P1639-0"},
		   {516123  ,  14  ,  "P1642-0"},
		   {516124  ,  14  ,  "P2641-0"},
		   {516125  ,  14  ,  "P2643-0"},
		   {516126  ,  14  ,  "P2645-0"},
		   {516127  ,  14  ,  "P2646-0"},
		   {516128  ,  14  ,  "P2647-0"},
		   {516129  ,  14  ,  "P2648-0"},
		   {516130  ,  14  ,  "P2640-0"},
		   {516131  ,  14  ,  "P2654-0"},
		   {516132  ,  14  ,  "P2650-0"},
		   {516133  ,  14  ,  "P2651-0"},
		   {516134  ,  14  ,  "P2652-0"},
		   {516135  ,  14  ,  "P2649-0"},
		   {516136  ,  14  ,  "P2655-0"},
		   {516137  ,  14  ,  "P2656-0"},
		   {516138  ,  14  ,  "P2657-0"},
		   {516139  ,  14  ,  "P2658-0"},
		   {516140  ,  14  ,  "P2659-0"},
		   {516141  ,  14  ,  "P2660-0"},
		   {516142  ,  14  ,  "P2661-0"},
		   {516143  ,  14  ,  "P2662-0"},
		   {516144  ,  14  ,  "P2663-0"},
		   {516145  ,  14  ,  "P2653-0"},
		   {516146  ,  14  ,  "P2664-0"},
		   {516147  ,  14  ,  "P2665-0"},
		   {516148  ,  14  ,  "P2666-0"},
		   {516149  ,  14  ,  "P2667-0"},
		   {516150  ,  14  ,  "P2674-0"},
		   {516151  ,  14  ,  "P2668-0"},
		   {516152  ,  14  ,  "P2669-0"},
		   {516153  ,  14  ,  "P2670-0"},
		   {516154  ,  14  ,  "P2672-0"},
		   {516155  ,  14  ,  "P2673-0"},
		   {516156  ,  14  ,  "P2679-0"},
		   {516157  ,  14  ,  "P2675-0"},
		   {516158  ,  14  ,  "P2676-0"},
		   {516159  ,  14  ,  "P2677-0"},
		   {516159  ,  31  ,  "P606-0"},
		   {516160  ,  14  ,  "P2678-0"},
		   {516160  ,  31  ,  "P603-0"},
		   {516161  ,  14  ,  "P2680-0"},
		   {516161  ,  31  ,  "P64C-0"},
		   {516162  ,  14  ,  "P2681-0"},
		   {516162  ,  31  ,  "P065C-0"},
		   {516163  ,  14  ,  "P2682-0"},
		   {516163  ,  31  ,  "P66C-0"},
		   {516164  ,  14  ,  "P2671-0"},
		   {516164  ,  31  ,  "P67C-0"},
		   {516165  ,  14  ,  "P2684-0"},
		   {516165  ,  31  ,  "P68C-0"},
		   {516166  ,  14  ,  "P2685-0"},
		   {516166  ,  31  ,  "P305D-0"},
		   {516167  ,  14  ,  "P2686-0"},
		   {516167  ,  31  ,  "U897-0"},
		   {516168  ,  14  ,  "P2683-0"},
		   {516168  ,  31  ,  "U898-0"},
		   {516169  ,  14  ,  "P2690-0"},
		   {516169  ,  31  ,  "U899-0"},
		   {516170  ,  14  ,  "P2687-0"},
		   {516171  ,  14  ,  "P2688-0"},
		   {516172  ,  14  ,  "P2689-0"},
		   {516173  ,  14  ,  "P2691-0"},
		   {516174  ,  14  ,  "P2692-0"},
		   {516175  ,  14  ,  "P2693-0"},
		   {516176  ,  14  ,  "P2694-0"},
		   {516177  ,  14  ,  "P2699-0"},
		   {516178  ,  31  ,  "P160C-0"},
		   {516179  ,  11  ,  "U161C-0"},
		   {516180  ,  31  ,  "P162C-0"},
		   {516181  ,  3  ,   "P63C-0"},
		   {516201  ,  2  ,   "P269A-0"},
		   {516202  ,  20  ,  "P20EE-0"},
		   {516203  ,  21  ,  "P2BAA-0"},
		   {522009  ,  2  ,   "UC59F-0"},
		   {522009  ,  12  ,  "UC29E-0"},
		   {522100  ,  2  ,   "UC59E-0"},
		   {522100  ,  12  ,  "UC29D-0"},
		   {522120  ,  2  ,   "UC282-0"},
		   {522122  ,  2  ,   "UC5A8-0"},
		   {522122  ,  12  ,  "UC2A1-0"},
		   {523017  ,  20  ,  "P15F8-0"},


};


// Service regen Macros for new algorithm.
#define SERVICE_REGEN_ACTIVE 	1U
#define REGEN_COMPLETE 			3U
#define REGEN_FAILED 			4U



static uint16_t FaultArrayIndex_Koel74[eDtc74hpKoelTextMaxNumber] = {0};
static uint16_t FaultArrayIndex_Tata74[eDtc74hpTataTextMaxNumber] = {0};
static uint16_t FaultArrayIndex_MandM74[eDtc74hpMandMTextMaxNumber] = {0};

uint32_t DisplaySpnValue 	= 0u;
uint8_t DisplayFmiValue 	= 0u;
static uint32_t TimeStampDelay10seconds = 0u;
static void _vDm1EMSTextWarning74HpKoel(void);
static void _vDm1EMSTextWarning74HpTata(void);
static void _vDm1EMSTextWarning74HpMandM(void);

uint8_t u8GetDmsTextStatus();
uint8_t u8GetStartStopTextStatus();
static void CheckCANPopUpMessage(void);
static uint8_t SinglePacketPcodeMatching = 0;
uint16_t PcodeIndex = 0u;
uint8_t PopUpMessageIndex = 1u;


uint8_t MsgTriggerFlag = 0u;
uint8_t msgdispCount = ePopUpDO_MANUAL_RGN;
static uint32_t MsgDisplayTime = 0u;
//uint8_t ActiveMsg[ePopUpTotal];
static uint32_t u32TimeStamp = 0U;
static uint8_t	TopLineMsgNo = 0u;
uint16_t Pcodes = 0;
uint16_t u16TotalFaults = 0U;
static uint8_t SPN_3719_FMI_16_FlagMultiPacket = 0u;
static void MultiplePopupDO_SERVICE_RGN(void);

static eSP2Faults eSP2Index = eSP2Blank;
static eSP2Faults eSP2FaultArray[eTotalSP2Faults];
static uint8_t bSP2FirstEntryFlag = FALSE;


/*********************************************************************/ /**
*
* Read All DTCs and raise the fault flags according to availability.
*
* @param	  None
*
* @return	  None
*************************************************************************/
void vSetEMSFaultText(void)
{
	if(GetEngineSelectionType() == eBackhoeLoaderKoelEngine74Hp)
	{
		_vDm1EMSTextWarning74HpKoel();
	}
	else if(GetEngineSelectionType() == eBackhoeLoaderEngineTata74Hp )
	{
		_vDm1EMSTextWarning74HpTata();
	}
	else if(GetEngineSelectionType() == eBackhoeLoaderEngineM_M74Hp)
	{
		_vDm1EMSTextWarning74HpMandM();
	}
//	else
//	{
//		_vDm1EMSTextWarning74HpKoel();	/* in case wrong value received  from flash , 74Hptata will work */
//	}

}



/*********************************************************************/ /**
 *
 * Read All DTCs 74Hp Koel and raise the fault flags according to availability.
 *
 * @param	  None
 *
 * @return	  None
 *************************************************************************/
static void _vDm1EMSTextWarning74HpKoel(void)
{
    uint16_t u16TotalSpns = 0U;
    uint32_t TempVar = 0U;
    uint16_t TempVar1 = 0U;
    static uint16_t FaultArrayListIndexNo = 0;
    static uint8_t ListPreparedFlag = 0;
    uint16_t PreparedListCounter = 0;
    static uint16_t OldTotalListNumber = 0U;//for clearing the old unncessary values
    static uint8_t u8Length = 0U;
    static uint8_t SinglePacketMessageFlag = 0;



    if((bGetSPNDataReady(eJDM1DTC_spn1) == FALSE) && (bGetPGNTimeOut(eJMil2_SysOkDM1_987) == TRUE) )
	{
    	 u16TotalFaults = 0u;
		 u16TotalSpns = 0u;
		 FaultArrayListIndexNo = 0u;
		 ListPreparedFlag = FALSE;
		 SinglePacketMessageFlag = FALSE;
		 SinglePacketPcodeMatching = FALSE;
		 for(uint16_t ClearIndex = FaultArrayListIndexNo ; ClearIndex <= eDtc74hpKoelTextMaxNumber; ClearIndex++)
		 {
		 	FaultArrayIndex_Koel74[ClearIndex] = 0u;
		 }
		 for(uint16_t ClearIndex = 0u; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
		 {
			DTC_J1939[ClearIndex].SPNValue = 0u;
			DTC_J1939[ClearIndex].FMICode = 0u;
		 }
	}

	/***************************For Multi frame Msgs**********************************************************/
	if ((u8GetDTCStatus() == 1) || (bGetSPNDataReady(eJDM1DTC_spn1)))
	{
		// If any of the above condition is True, that means PCOdes are available on DM1
		// if Multiframe status is True.
		if ( ((u8GetDTCStatus() == 1) && (bGetPGNTimeOut(eJMil2_SysOkDM1_987) == 0)) )
		{
			SinglePacketMessageFlag = FALSE;
			vClearSPNDataReady(eJDM1DTC_spn1);
			u16TotalSpns = u16GetDtcTotalBytes(); // Get Total Number of Data BYtes in mutiframe
			if (u16TotalSpns > 8U) // If total number bytes is more than 8(for a valid multiframe)
			{
				u16TotalSpns = (u16TotalSpns - 2U) / 4U;  // Calculate total no. of SPN and FMI combination
				if (u16TotalSpns > MAXIMUM_NUMBER_OF_DTC) // Capping total number to 38
				{
					u16TotalSpns = MAXIMUM_NUMBER_OF_DTC;
				}
			}
			else // If multifrmae is not available then Single might be available.
			{
				u16TotalSpns = 0U; // Keep this zero to count only one SPN and FMI.
			}
			u16TotalFaults = u16TotalSpns;

			if ((u16TotalSpns > 0U))// && (u8Length <= (uint8_t)u16TotalSpns))
			{
				//u8Length = u16TotalSpns;
				if(u16TotalSpns > MAXIMUM_NUMBER_OF_DTC)
				{
					u16TotalSpns = MAXIMUM_NUMBER_OF_DTC;
				}
			}
		}
        else if (bGetSPNDataReady(eJDM1DTC_spn1) && u8GetDTCStatus() == 0) // No single Frame TimeOut PGN 65226 SA 00
        {
            // Extract SPN value from DM1 single frame.
            TempVar = (u32GetPGNdata(eJDM1DTC_spn1)) | (u32GetPGNdata(eJDM1DTC_spn2) << 8) | ((u32GetPGNdata(eJDM1DTC_fmi) & 0xE0U) << 11);
            // Get FMI data from DM1 single frame.
            TempVar1 = (uint8_t)(u32GetPGNdata(eJDM1DTC_fmi) & 0x1FU);
            SinglePacketMessageFlag = TRUE;
            u16TotalFaults = 0u;
        }
        else
        {
            ; /* do nothing*/
        }


		u8Length = 0;
        while (u8Length <= u16TotalSpns)
        {
        	HAL_IWDG_Refresh(&hiwdg);
			if (ListPreparedFlag == 1u)
			{
				if( (u16TotalSpns == FaultArrayListIndexNo))//  &&  ((u32GetClockTicks() - ListPrepIterTimeForSameMsgs) > 800)  ) //Case for same number of msgs but different content
				{
					for(uint16_t PreparedIndex = 0;PreparedIndex < u16TotalSpns;PreparedIndex++)
					{
						for(uint16_t FaultIndex = 0;FaultIndex < u16TotalSpns;FaultIndex++)
						{
							/* SPN and FMI Value will be implement ---------------------*/
							if(  (((DtcSpnNFmiTableKoel74Hp[FaultArrayIndex_Koel74[PreparedIndex]].SPN == DTC_J1939[FaultIndex].SPNValue) && (DTC_J1939[FaultIndex].SPNValue != 0)) &&
									(DtcSpnNFmiTableKoel74Hp[FaultArrayIndex_Koel74[PreparedIndex]].FMI == DTC_J1939[FaultIndex].FMICode))
									||
									(( (DtcSpnNFmiTableKoel74Hp[FaultArrayIndex_Koel74[PreparedIndex]].SPN == TempVar) && (TempVar != 0))  &&
									(DtcSpnNFmiTableKoel74Hp[FaultArrayIndex_Koel74[PreparedIndex]].FMI == TempVar1))  )
							{
								++PreparedListCounter;
							}
						}
					}

					if(PreparedListCounter == FaultArrayListIndexNo)
					{
						PreparedListCounter = 0;
					}
					else
					{
						FaultArrayListIndexNo = 0; //reset the index for fresh values
						ListPreparedFlag = 0;

						 for(uint16_t ClearIndex = 0; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
						 {
							DTC_J1939[ClearIndex].SPNValue = 0;
							DTC_J1939[ClearIndex].FMICode = 0;
							vResetDTCParameters();
							SinglePacketMessageFlag = FALSE;
						 }
					}
				}
				else if(u16TotalSpns != FaultArrayListIndexNo)//case for completely fresh values
				{
					FaultArrayListIndexNo = 0; //reset the index for fresh values
					ListPreparedFlag = 0;
					 for(uint16_t ClearIndex = 0; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
					 {
						DTC_J1939[ClearIndex].SPNValue = 0;
						DTC_J1939[ClearIndex].FMICode = 0;
						vResetDTCParameters();
					 }
				}
				else
				{
					;//do nothing
				}
			}
			else
			{
				for (uint16_t Index = 0; Index < eDtc74hpKoelTextMaxNumber; Index++)//Loop to fill the new values
				{
					if ( (((DtcSpnNFmiTableKoel74Hp[Index].SPN == DTC_J1939[u8Length].SPNValue) && (DTC_J1939[u8Length].SPNValue != 0))
						&& (DtcSpnNFmiTableKoel74Hp[Index].FMI == DTC_J1939[u8Length].FMICode))
						||
					(((DtcSpnNFmiTableKoel74Hp[Index].SPN == TempVar) && (TempVar != 0))
					&& (DtcSpnNFmiTableKoel74Hp[Index].FMI == TempVar1)) )
					{
						FaultArrayIndex_Koel74[FaultArrayListIndexNo] = Index;
						FaultArrayListIndexNo++;
						SinglePacketPcodeMatching = TRUE;
						if(FaultArrayListIndexNo == u16TotalSpns || SinglePacketMessageFlag == TRUE)//list is now prepared)
						{
							ListPreparedFlag = 1;	/* List is now prepared */
							if(OldTotalListNumber > FaultArrayListIndexNo)
							{
								for(int ClearIndex = FaultArrayListIndexNo ; ClearIndex <= OldTotalListNumber; ClearIndex++)
								{
									FaultArrayIndex_Koel74[ClearIndex] = 0;
								}
							}
							else
							{
								OldTotalListNumber = FaultArrayListIndexNo;
							}
						}
						break;
					}
					else
					{
						//; //do nothing
						SinglePacketPcodeMatching = FALSE; // only for single packet
					}
				}
			}
			MultiplePopupDO_SERVICE_RGN();
            if ((u8Length >= (uint8_t)u16TotalSpns) && (u8Length != 0U))
            {
                vClearDTC();
            }
			u8Length++;
		}
    }

}



/*********************************************************************/ /**
 *
 * Read All DTCs 74 Hp MandM and raise the fault flags according to availability.
 *
 * @param	  None
 *
 * @return	  None
 *************************************************************************/
static void _vDm1EMSTextWarning74HpMandM(void)
{
    uint16_t u16TotalSpns = 0U;
    uint32_t TempVar = 0U;
    uint16_t TempVar1 = 0U;
    static uint16_t FaultArrayListIndexNo = 0;
    static uint8_t ListPreparedFlag = 0;
    uint16_t PreparedListCounter = 0;
    static uint16_t OldTotalListNumber = 0U;//for clearing the old unncessary values
    static uint8_t u8Length = 0U;
    static uint8_t SinglePacketMessageFlag = 0;



    if((bGetSPNDataReady(eJDM1DTC_spn1) == FALSE) && (bGetPGNTimeOut(eJMil2_SysOkDM1_987) == TRUE) )
	{
    	 u16TotalFaults = 0u;
		 u16TotalSpns = 0u;
		 FaultArrayListIndexNo = 0u;
		 ListPreparedFlag = FALSE;
		 SinglePacketMessageFlag = FALSE;
		 SinglePacketPcodeMatching = FALSE;
		 for(uint16_t ClearIndex = FaultArrayListIndexNo ; ClearIndex <= eDtc74hpMandMTextMaxNumber; ClearIndex++)
		 {
			 FaultArrayIndex_MandM74[ClearIndex] = 0u;
		 }
		 for(uint16_t ClearIndex = 0u; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
		 {
			DTC_J1939[ClearIndex].SPNValue = 0u;
			DTC_J1939[ClearIndex].FMICode = 0u;
		 }
	}

	/***************************For Multi frame Msgs**********************************************************/
	if ((u8GetDTCStatus() == 1) || (bGetSPNDataReady(eJDM1DTC_spn1)))
	{
		// If any of the above condition is True, that means PCOdes are available on DM1
		// if Multiframe status is True.
		if ( ((u8GetDTCStatus() == 1) && (bGetPGNTimeOut(eJMil2_SysOkDM1_987) == 0)) )
		{
			SinglePacketMessageFlag = FALSE;
			vClearSPNDataReady(eJDM1DTC_spn1);
			u16TotalSpns = u16GetDtcTotalBytes(); // Get Total Number of Data BYtes in mutiframe
			if (u16TotalSpns > 8U) // If total number bytes is more than 8(for a valid multiframe)
			{
				u16TotalSpns = (u16TotalSpns - 2U) / 4U;  // Calculate total no. of SPN and FMI combination
				if (u16TotalSpns > MAXIMUM_NUMBER_OF_DTC) // Capping total number to 38
				{
					u16TotalSpns = MAXIMUM_NUMBER_OF_DTC;
				}
			}
			else // If multifrmae is not available then Single might be available.
			{
				u16TotalSpns = 0U; // Keep this zero to count only one SPN and FMI.
			}
			u16TotalFaults = u16TotalSpns;

			if ((u16TotalSpns > 0U))// && (u8Length <= (uint8_t)u16TotalSpns))
			{
				//u8Length = u16TotalSpns;
				if(u16TotalSpns > MAXIMUM_NUMBER_OF_DTC)
				{
					u16TotalSpns = MAXIMUM_NUMBER_OF_DTC;
				}
			}
		}
        else if (bGetSPNDataReady(eJDM1DTC_spn1) && u8GetDTCStatus() == 0) // No single Frame TimeOut PGN 65226 SA 00
        {
            // Extract SPN value from DM1 single frame.
            TempVar = (u32GetPGNdata(eJDM1DTC_spn1)) | (u32GetPGNdata(eJDM1DTC_spn2) << 8) | ((u32GetPGNdata(eJDM1DTC_fmi) & 0xE0U) << 11);
            // Get FMI data from DM1 single frame.
            TempVar1 = (uint8_t)(u32GetPGNdata(eJDM1DTC_fmi) & 0x1FU);
            SinglePacketMessageFlag = TRUE;
            u16TotalFaults = 0u;
        }
        else
        {
            ; /* do nothing*/
        }


		u8Length = 0;
        while (u8Length <= u16TotalSpns)
        {
        	HAL_IWDG_Refresh(&hiwdg);
			if (ListPreparedFlag == 1u)
			{
				if( (u16TotalSpns == FaultArrayListIndexNo))//  &&  ((u32GetClockTicks() - ListPrepIterTimeForSameMsgs) > 800)  ) //Case for same number of msgs but different content
				{
					for(uint16_t PreparedIndex = 0;PreparedIndex < u16TotalSpns;PreparedIndex++)
					{
						for(uint16_t FaultIndex = 0;FaultIndex < u16TotalSpns;FaultIndex++)
						{
							/* SPN and FMI Value will be implement ---------------------*/
							if(  (((DtcSpnNFmiTableMandM74Hp[FaultArrayIndex_MandM74[PreparedIndex]].SPN == DTC_J1939[FaultIndex].SPNValue) && (DTC_J1939[FaultIndex].SPNValue != 0)) &&
									(DtcSpnNFmiTableMandM74Hp[FaultArrayIndex_MandM74[PreparedIndex]].FMI == DTC_J1939[FaultIndex].FMICode))
									||
									(( (DtcSpnNFmiTableMandM74Hp[FaultArrayIndex_MandM74[PreparedIndex]].SPN == TempVar) && (TempVar != 0))  &&
									(DtcSpnNFmiTableMandM74Hp[FaultArrayIndex_MandM74[PreparedIndex]].FMI == TempVar1))  )
							{
								++PreparedListCounter;
							}
						}
					}

					if(PreparedListCounter == FaultArrayListIndexNo)
					{
						PreparedListCounter = 0;
					}
					else
					{
						FaultArrayListIndexNo = 0; //reset the index for fresh values
						ListPreparedFlag = 0;

						 for(uint16_t ClearIndex = 0; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
						 {
							DTC_J1939[ClearIndex].SPNValue = 0;
							DTC_J1939[ClearIndex].FMICode = 0;
							vResetDTCParameters();
							SinglePacketMessageFlag = FALSE;
						 }
					}
				}
				else if(u16TotalSpns != FaultArrayListIndexNo)//case for completely fresh values
				{
					FaultArrayListIndexNo = 0; //reset the index for fresh values
					ListPreparedFlag = 0;
					 for(uint16_t ClearIndex = 0; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
					 {
						DTC_J1939[ClearIndex].SPNValue = 0;
						DTC_J1939[ClearIndex].FMICode = 0;
						vResetDTCParameters();
					 }
				}
				else
				{
					;//do nothing
				}
			}
			else
			{
				for (uint16_t Index = 0; Index < eDtc74hpMandMTextMaxNumber; Index++)//Loop to fill the new values
				{
					if ( (((DtcSpnNFmiTableMandM74Hp[Index].SPN == DTC_J1939[u8Length].SPNValue) && (DTC_J1939[u8Length].SPNValue != 0))
						&& (DtcSpnNFmiTableMandM74Hp[Index].FMI == DTC_J1939[u8Length].FMICode))
						||
					(((DtcSpnNFmiTableMandM74Hp[Index].SPN == TempVar) && (TempVar != 0))
					&& (DtcSpnNFmiTableMandM74Hp[Index].FMI == TempVar1)) )
					{
						FaultArrayIndex_MandM74[FaultArrayListIndexNo] = Index;
						FaultArrayListIndexNo++;
						SinglePacketPcodeMatching = TRUE;
						if(FaultArrayListIndexNo == u16TotalSpns || SinglePacketMessageFlag == TRUE)//list is now prepared)
						{
							ListPreparedFlag = 1;	/* List is now prepared */
							if(OldTotalListNumber > FaultArrayListIndexNo)
							{
								for(int ClearIndex = FaultArrayListIndexNo ; ClearIndex <= OldTotalListNumber; ClearIndex++)
								{
									FaultArrayIndex_MandM74[ClearIndex] = 0;
								}
							}
							else
							{
								OldTotalListNumber = FaultArrayListIndexNo;
							}
						}
						break;
					}
					else
					{
						//; //do nothing
						SinglePacketPcodeMatching = FALSE; // only for single packet
					}
				}
			}
			MultiplePopupDO_SERVICE_RGN();
            if ((u8Length >= (uint8_t)u16TotalSpns) && (u8Length != 0U))
            {
                vClearDTC();
            }
			u8Length++;
		}
    }
}


/*********************************************************************/ /**
 *
 * Read All DTCs 101Hptata and raise the fault flags according to availability.
 *
 * @param	  None
 *
 * @return	  None
 *************************************************************************/
static void _vDm1EMSTextWarning74HpTata(void)
{
    uint16_t u16TotalSpns = 0U;
    uint32_t TempVar = 0U;
    uint16_t TempVar1 = 0U;
    static uint16_t FaultArrayListIndexNo = 0;
    static uint8_t ListPreparedFlag = 0;
    uint16_t PreparedListCounter = 0;
    static uint16_t OldTotalListNumber = 0U;//for clearing the old unncessary values
    static uint8_t u8Length = 0U;
    static uint8_t SinglePacketMessageFlag = 0;


    if((bGetSPNDataReady(eJDM1DTC_spn1) == FALSE) && (bGetPGNTimeOut(eJMil2_SysOkDM1_987) == TRUE) )
	{
    	 u16TotalFaults = 0u;
		 u16TotalSpns = 0u;
		 FaultArrayListIndexNo = 0u;
		 ListPreparedFlag = FALSE;
		 SinglePacketMessageFlag = FALSE;
		 SinglePacketPcodeMatching = FALSE;
		 for(uint16_t ClearIndex = FaultArrayListIndexNo ; ClearIndex <= eDtc74hpTataTextMaxNumber; ClearIndex++)
		 {
			 FaultArrayIndex_Tata74[ClearIndex] = 0u;
		 }
		 for(uint16_t ClearIndex = 0u; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
		 {
			DTC_J1939[ClearIndex].SPNValue = 0u;
			DTC_J1939[ClearIndex].FMICode = 0u;
		 }
	}

	/***************************For Multi frame Msgs**********************************************************/
	if ((u8GetDTCStatus() == 1) || (bGetSPNDataReady(eJDM1DTC_spn1)))
	{
		// If any of the above condition is True, that means PCOdes are available on DM1
		// if Multiframe status is True.
		if ( ((u8GetDTCStatus() == 1) && (bGetPGNTimeOut(eJMil2_SysOkDM1_987) == 0)) )
		{
			SinglePacketMessageFlag = FALSE;
			vClearSPNDataReady(eJDM1DTC_spn1);
			u16TotalSpns = u16GetDtcTotalBytes(); // Get Total Number of Data BYtes in mutiframe
			if (u16TotalSpns > 8U) // If total number bytes is more than 8(for a valid multiframe)
			{
				u16TotalSpns = (u16TotalSpns - 2U) / 4U;  // Calculate total no. of SPN and FMI combination
				if (u16TotalSpns > MAXIMUM_NUMBER_OF_DTC) // Capping total number to 38
				{
					u16TotalSpns = MAXIMUM_NUMBER_OF_DTC;
				}
			}
			else // If multifrmae is not available then Single might be available.
			{
				u16TotalSpns = 0U; // Keep this zero to count only one SPN and FMI.
			}
			u16TotalFaults = u16TotalSpns;

			if ((u16TotalSpns > 0U))// && (u8Length <= (uint8_t)u16TotalSpns))
			{
				//u8Length = u16TotalSpns;
				if(u16TotalSpns > MAXIMUM_NUMBER_OF_DTC)
				{
					u16TotalSpns = MAXIMUM_NUMBER_OF_DTC;
				}
			}
		}
        else if (bGetSPNDataReady(eJDM1DTC_spn1) && u8GetDTCStatus() == 0) // No single Frame TimeOut PGN 65226 SA 00
        {
            // Extract SPN value from DM1 single frame.
            TempVar = (u32GetPGNdata(eJDM1DTC_spn1)) | (u32GetPGNdata(eJDM1DTC_spn2) << 8) | ((u32GetPGNdata(eJDM1DTC_fmi) & 0xE0U) << 11);
            // Get FMI data from DM1 single frame.
            TempVar1 = (uint8_t)(u32GetPGNdata(eJDM1DTC_fmi) & 0x1FU);
            SinglePacketMessageFlag = TRUE;
            u16TotalFaults = 0u;
        }
        else
        {
            ; /* do nothing*/
        }


		u8Length = 0;
        while (u8Length <= u16TotalSpns)
        {
        	HAL_IWDG_Refresh(&hiwdg);
			if (ListPreparedFlag == 1u)
			{
				if( (u16TotalSpns == FaultArrayListIndexNo))//  &&  ((u32GetClockTicks() - ListPrepIterTimeForSameMsgs) > 800)  ) //Case for same number of msgs but different content
				{
					for(uint16_t PreparedIndex = 0;PreparedIndex < u16TotalSpns;PreparedIndex++)
					{
						for(uint16_t FaultIndex = 0;FaultIndex < u16TotalSpns;FaultIndex++)
						{
							/* SPN and FMI Value will be implement ---------------------*/
							if(  (((DtcSpnNFmiTableTata74Hp[FaultArrayIndex_Tata74[PreparedIndex]].SPN == DTC_J1939[FaultIndex].SPNValue) && (DTC_J1939[FaultIndex].SPNValue != 0)) &&
									(DtcSpnNFmiTableTata74Hp[FaultArrayIndex_Tata74[PreparedIndex]].FMI == DTC_J1939[FaultIndex].FMICode))
									||
									(( (DtcSpnNFmiTableTata74Hp[FaultArrayIndex_Tata74[PreparedIndex]].SPN == TempVar) && (TempVar != 0))  &&
									(DtcSpnNFmiTableTata74Hp[FaultArrayIndex_Tata74[PreparedIndex]].FMI == TempVar1))  )
							{
								++PreparedListCounter;
							}
						}
					}

					if(PreparedListCounter == FaultArrayListIndexNo)
					{
						PreparedListCounter = 0;
					}
					else
					{
						FaultArrayListIndexNo = 0; //reset the index for fresh values
						ListPreparedFlag = 0;

						 for(uint16_t ClearIndex = 0; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
						 {
							DTC_J1939[ClearIndex].SPNValue = 0;
							DTC_J1939[ClearIndex].FMICode = 0;
							vResetDTCParameters();
							SinglePacketMessageFlag = FALSE;
						 }
					}
				}
				else if(u16TotalSpns != FaultArrayListIndexNo)//case for completely fresh values
				{
					FaultArrayListIndexNo = 0; //reset the index for fresh values
					ListPreparedFlag = 0;
					 for(uint16_t ClearIndex = 0; ClearIndex <= NO_OF_DTC_CODE; ClearIndex++)
					 {
						DTC_J1939[ClearIndex].SPNValue = 0;
						DTC_J1939[ClearIndex].FMICode = 0;
						vResetDTCParameters();
					 }
				}
				else
				{
					;//do nothing
				}
			}
			else
			{
				for (uint16_t Index = 0; Index < eDtc74hpTataTextMaxNumber; Index++)//Loop to fill the new values
				{
					if ( (((DtcSpnNFmiTableTata74Hp[Index].SPN == DTC_J1939[u8Length].SPNValue) && (DTC_J1939[u8Length].SPNValue != 0))
						&& (DtcSpnNFmiTableTata74Hp[Index].FMI == DTC_J1939[u8Length].FMICode))
						||
					(((DtcSpnNFmiTableTata74Hp[Index].SPN == TempVar) && (TempVar != 0))
					&& (DtcSpnNFmiTableTata74Hp[Index].FMI == TempVar1)) )
					{
						FaultArrayIndex_Tata74[FaultArrayListIndexNo] = Index;
						FaultArrayListIndexNo++;
						SinglePacketPcodeMatching = TRUE;
						if(FaultArrayListIndexNo == u16TotalSpns || SinglePacketMessageFlag == TRUE)//list is now prepared)
						{
							ListPreparedFlag = 1;	/* List is now prepared */
							if(OldTotalListNumber > FaultArrayListIndexNo)
							{
								for(int ClearIndex = FaultArrayListIndexNo ; ClearIndex <= OldTotalListNumber; ClearIndex++)
								{
									FaultArrayIndex_Tata74[ClearIndex] = 0;
								}
							}
							else
							{
								OldTotalListNumber = FaultArrayListIndexNo;
							}
						}
						break;
					}
					else
					{
						//; //do nothing
						SinglePacketPcodeMatching = FALSE; // only for single packet
					}
				}
			}
			MultiplePopupDO_SERVICE_RGN();
            if ((u8Length >= (uint8_t)u16TotalSpns) && (u8Length != 0U))
            {
                vClearDTC();
            }
			u8Length++;
		}
    }

}

/*********************************************************************/ /**
 *
 * Read All DTCs 130 AL  and raise the fault flags according to availability.
 *
 * @param	  None
 *
 * @return	  None
 *************************************************************************/


void PopUpMessageDisplayTFT(void)
{
	uint8_t i = 0u;
	static uint8_t bWait = FALSE;

	CheckCANPopUpMessage();			/* Text msg from id - 0CFF0200 D8 pos. */

    if (bWait == FALSE)
    {
        u32TimeStamp = u32GetClockTicks();
        bWait = TRUE;
    }
    else
    {

        /*Do Nothing*/
    }
    // Total Number of SP2 texts available is 25
    if (eSP2Index < eTotalSP2Faults)
    {
        if (((u32GetClockTicks() - u32TimeStamp) < MSG_ROTATE_TIME) && (eSP2FaultArray[eSP2Index] != 0u))
        {
			if(bSP2FirstEntryFlag == TRUE)
			{
				SetTopLineMsgNumber(eSP2FaultArray[eSP2Index]);
			}
			bSP2FirstEntryFlag = TRUE;
        }
        else // If text is blank or 4 seconds interval finished increment the Text Index.
        {
            eSP2Index++;
            u32TimeStamp = u32GetClockTicks();
        }
    }
    else
    {
    	SetTopLineMsgNumber(eSP2FaultArray[eSP2Blank]);
        eSP2Index = 0U;
        for (i = 0; i < (uint8_t)eTotalSP2Faults; i++)
        {
            eSP2FaultArray[i] = 0u;
        }
    }

}
/*************************************************************************************************************//**
 *
 * Purpose  :  MultiplePopupDO_SERVICE_RGN , This function Implemented for check pop up message with Multiple message
 *
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 ****************************************************************************************************************/
static void MultiplePopupDO_SERVICE_RGN(void)
{
	uint8_t DtcLength = 0;

	for(DtcLength = 0; DtcLength < MAXIMUM_NUMBER_OF_DTC; DtcLength++)
	{
		if((DTC_J1939[DtcLength].SPNValue == DTC_SPN3719) && (DTC_J1939[DtcLength].FMICode == DTC_FMI16))
		{
			SPN_3719_FMI_16_FlagMultiPacket = TRUE;
		}
		else
		{
			/*NA*/
		}
	}
}
/*************************************************************************************************************//**
 *
 * Purpose  :  CheckCANPopUpMessage , This function Implemented for check Text Msg for ID 0x0CFF0200
 *
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 ****************************************************************************************************************/
static void CheckCANPopUpMessage(void)
{
	uint8_t dataSPN_3697 = 0u;
	uint8_t dataSPN_3703 = 0u;
	uint8_t dataSPN_4175 = 0u;
	uint8_t dataSPN_3700 = 0u;
	uint8_t dataSPN_3704 = 0u;
	uint8_t dataSPN_3705 = 0u;
	uint8_t dataSPN_3707 = 0u;
	uint8_t dataSPN_3708 = 0u;
	uint8_t dataSPN_3710 = 0u;
	uint8_t dataSPN_3711 = 0u;
	uint8_t dataSPN_3712 = 0u;
	uint8_t dataSPN_3716 = 0u;
	uint8_t i = 0;

	static uint8_t bRegenStopPressed = FALSE;
    static uint32_t u32RegenCompleteTime = 0U;
    static uint32_t u32RegenFailedTime = 0U;
	static uint32_t SPN_SinglePacketVal = 0;
	static uint8_t FMI_SinglePacketVal = 0;
	static uint8_t SPN_3719_FMI_16_FlagSingle = 0;
    static uint8_t u8SPN4175Status = 0U;
    static uint8_t u8ServiceRegenState = 0U;

	if(!bGetPGNTimeOut(ePopupMessageSPN_4175))
	{
		/* Take CAN raw data --------------------------------------------------*/
		dataSPN_3697 = (uint8_t)u32GetPGNdata(eDpfLampIndicationLampSPN_3697);
		dataSPN_3703 = (uint8_t)u32GetPGNdata(eDpfRegenInhibitLampSPN_3703);
	   	dataSPN_4175 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_4175);
	   	dataSPN_3700 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3700);
		dataSPN_3704 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3704);
		dataSPN_3705 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3705);
		dataSPN_3707 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3707);
		dataSPN_3708 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3708);
		dataSPN_3710 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3710);
		dataSPN_3711 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3711);
		dataSPN_3712 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3712);
		dataSPN_3716 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3716);


		if((dataSPN_4175 == 0u && dataSPN_3700 != 1u) && (dataSPN_3697 == 4u || dataSPN_3697 == 5u))
		{
			eSP2FaultArray[ePopUpDO_MANUAL_RGN] = ePopUpDO_MANUAL_RGN;
		}
		else
		{
			eSP2FaultArray[ePopUpDO_MANUAL_RGN] = eSP2Blank;
		}

		if(dataSPN_3703 == 1u)
		{
			eSP2FaultArray[ePopUpRGN_STOP_PRESSED] = ePopUpRGN_STOP_PRESSED;
		}
		else
		{
			eSP2FaultArray[ePopUpRGN_STOP_PRESSED] = eSP2Blank;
		}


		if(dataSPN_4175 == 1u)
		{
			eSP2FaultArray[ePopUp_MANUAL_RGN_ACTIVE] = ePopUp_MANUAL_RGN_ACTIVE;
            u32RegenCompleteTime = u32GetClockTicks();
            u32RegenFailedTime = u32GetClockTicks();
            u8SPN4175Status = SERVICE_REGEN_ACTIVE; // Service regen status is TRUE.
		}
		else if ((dataSPN_4175 == 0U) && (u8SPN4175Status == SERVICE_REGEN_ACTIVE)) // SPN 4175 transitions from // 0b001 to 0b000
		{
			for (i = 0; i < eTotalSP2Faults; i++)
			{
				eSP2FaultArray[i] = eSP2Blank;
			}
			if (((u32GetClockTicks() - u32RegenCompleteTime) <= FIVE_SECONDS) && ((u32GetClockTicks() - u32RegenFailedTime) <= FIVE_SECONDS)) // For 2 Seconds keep monitoring
			{
				if((dataSPN_3703 != 1) && (dataSPN_3704 != 1) && (dataSPN_3705 != 1) && (dataSPN_3708 != 1) && (dataSPN_3710 != 1) && (dataSPN_3711 != 1) && (dataSPN_3712 != 1) && (dataSPN_3716 != 1) && (dataSPN_3707 != 1)) // if Zero then KEEP status as
				{                                                                                                                                                                            // RegenComplete
					u8ServiceRegenState = REGEN_COMPLETE;
					u32RegenFailedTime = u32GetClockTicks(); // Update RegenFailed TimeStamp
				}
				else if(dataSPN_3703 == 1u || dataSPN_3704 == 1u || dataSPN_3705 == 1u || dataSPN_3707 == 1u || dataSPN_3708 == 1u || dataSPN_3710 == 1u || dataSPN_3711 == 1u || dataSPN_3712 == 1u || dataSPN_3716 == 1u)
				{
					u8ServiceRegenState = REGEN_FAILED;
					u32RegenCompleteTime = u32GetClockTicks(); // Update RegenComplete TimeStamp
				}
			}
		}
        // This Code determines that after monitoring for 2 Seconds which state was fixed for 2 Seconds.
        // To decide The timestamp for Regen Failed and Complete is compared and Service Regen State is
        // compared with REGEN_COMPLETE and REGEN_FAILED.
        if ((u8ServiceRegenState == REGEN_COMPLETE) && ((u32GetClockTicks() - u32RegenCompleteTime) > FIVE_SECONDS))
        {
            u8ServiceRegenState = FALSE;
            u8SPN4175Status = FALSE;
            u32RegenCompleteTime = u32GetClockTicks();
            eSP2FaultArray[ePopUpMANUAL_RGN_COMPLETE] = ePopUpMANUAL_RGN_COMPLETE;
            eSP2FaultArray[ePopUp_MANUAL_RGN_ACTIVE] = eSP2Blank;
            eSP2FaultArray[ePopUpMANUAL_RGN_ABORT] = eSP2Blank;
            eSP2Index = ePopUpMANUAL_RGN_COMPLETE;
    		u32TimeStamp = u32GetClockTicks();
        }
        else if ((u8ServiceRegenState == REGEN_FAILED) && ((u32GetClockTicks() - u32RegenFailedTime) > FIVE_SECONDS))
        {
            u8ServiceRegenState = FALSE;
            u8SPN4175Status = FALSE;
            u32RegenFailedTime = u32GetClockTicks();
            eSP2FaultArray[ePopUpMANUAL_RGN_COMPLETE] = eSP2Blank;
            eSP2FaultArray[ePopUp_MANUAL_RGN_ACTIVE] = eSP2Blank;
            eSP2FaultArray[ePopUpMANUAL_RGN_ABORT] = ePopUpMANUAL_RGN_ABORT;
            eSP2Index = ePopUpMANUAL_RGN_ABORT;
            u32TimeStamp = u32GetClockTicks();
        }
        else
        {
        	/*Do Nothing*/
        }

        // Clear All faults related to Regen service afetr it is Displayed once or Echoed Once by
        if ((u32GetClockTicks() - u32RegenCompleteTime) > FOUR_SECONDS)
        {
        	eSP2FaultArray[ePopUpMANUAL_RGN_COMPLETE] = eSP2Blank;
        }
        else if ((u32GetClockTicks() - u32RegenFailedTime) > FOUR_SECONDS)
        {
        	eSP2FaultArray[ePopUpMANUAL_RGN_ABORT] = eSP2Blank;
        }
        else
        {
        	/*Do Nothing*/
        }
        // Checking the transition for Regeneration Stop Pressed.
        // Refer Document "BSVI_IC_LCD-Text Message -V1.1_09112019.xls ".
	}
	else
	{
		eSP2FaultArray[ePopUpDO_MANUAL_RGN] = eSP2Blank;
		eSP2FaultArray[ePopUp_MANUAL_RGN_ACTIVE] = eSP2Blank;
		eSP2FaultArray[ePopUpMANUAL_RGN_ABORT] = eSP2Blank;
		eSP2FaultArray[ePopUpRGN_STOP_PRESSED] = eSP2Blank;
		eSP2FaultArray[ePopUpMANUAL_RGN_COMPLETE] = eSP2Blank;
        u8ServiceRegenState = FALSE;
        bRegenStopPressed = FALSE;
        u8SPN4175Status = FALSE;
	}

	if(bGetPGNTimeOut(eJDM1DTC_spn1) == TRUE)
	{
		SPN_SinglePacketVal = 0u;
		FMI_SinglePacketVal = 0u;
		SPN_3719_FMI_16_FlagSingle = FALSE;
		vClearSPNDataReady(eJDM1DTC_spn1);
	}
	else
	{
		SPN_SinglePacketVal = (u32GetPGNdata(eJDM1DTC_spn1)) | (u32GetPGNdata(eJDM1DTC_spn2) << 8) | ((u32GetPGNdata(eJDM1DTC_fmi) & 0xE0U) << 11);
		// Get FMI data from DM1 single frame.
		FMI_SinglePacketVal = (uint8_t)(u32GetPGNdata(eJDM1DTC_fmi) & 0x1FU);
	}


	if((SPN_SinglePacketVal == DTC_SPN3719) && (FMI_SinglePacketVal == DTC_FMI16))
	{
		SPN_3719_FMI_16_FlagSingle = TRUE;
	}
	else
	{
		SPN_3719_FMI_16_FlagSingle = FALSE;
	}


	if((SPN_3719_FMI_16_FlagSingle == TRUE) || (SPN_3719_FMI_16_FlagMultiPacket == TRUE))
	{
		SPN_3719_FMI_16_FlagMultiPacket = FALSE;
		SPN_3719_FMI_16_FlagSingle = FALSE;
		if((u32GetClockTicks() - TimeStampDelay10seconds <= 2500u))
		{
			eSP2FaultArray[ePopUpDO_SERVICE_RGN] = ePopUpDO_SERVICE_RGN;
		}
		TimeStampDelay10seconds = u32GetClockTicks();
	}
	else if((u32GetClockTicks() - TimeStampDelay10seconds >= 3000u))
	{
		eSP2FaultArray[ePopUpDO_SERVICE_RGN] = eSP2Blank;
	}

}

uint16_t getFaultNumber(void)
{
    return u16TotalFaults;
}

/**********************************************************************************//**
 *
 * Purpose  :  Set Top Line Msg Number
 * @param[in]      :uint8_t, Value for top line msg display
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
void SetTopLineMsgNumber(uint8_t Value)
{
	TopLineMsgNo = Value;
}

/**********************************************************************************//**
 *
 * Purpose  :  Get Top Line Msg Number
 * @param[in]      :None
 * @param[out]:	None
 * @return    : uint8_t, Value for top line msg display
 **************************************************************************************/
uint8_t GetSetTopLineMsgNumber(void)
{
	 return TopLineMsgNo;
}

/**************************************************************************************/
DTCLookupTable* GetPcodeSpnFmiChildScreen(void)
{
	if(GetEngineSelectionType() == eBackhoeLoaderKoelEngine74Hp )
	{
		return (&DtcSpnNFmiTableKoel74Hp[0]);
	}
	else if(GetEngineSelectionType() == eBackhoeLoaderEngineM_M74Hp)
	{
		return (&DtcSpnNFmiTableMandM74Hp[0]);
	}
	else
	{
		return (&DtcSpnNFmiTableTata74Hp[0]);	/* in case wrong value received  from flash , 74Hptata will work */
	}

}

uint16_t* GetFaultArrayIndexSpnFmiIndexNo(void)
{

	if(GetEngineSelectionType() == eBackhoeLoaderEngineTata74Hp)
	{
		return (&FaultArrayIndex_Tata74[0]);
	}
	else if(GetEngineSelectionType() == eBackhoeLoaderEngineM_M74Hp )
	{
		return (&FaultArrayIndex_MandM74[0]);
	}

	else
	{
		return (&FaultArrayIndex_Koel74[0]);	/* in case wrong value received  from flash , 74Hptata will work */
	}

}


uint8_t GetSinglePacketPcodeMatchingStatus()
{
	return SinglePacketPcodeMatching;
}


