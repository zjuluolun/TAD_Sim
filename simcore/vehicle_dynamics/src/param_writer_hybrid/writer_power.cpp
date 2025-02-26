// Copyright 2024 Tencent Inc. All rights reserved.
//

#include "inc/writer_power.h"
#include "inc/car_log.h"
#include "inc/proto_helper.h"

PowerParamWriter::PowerParamWriter() {}

void PowerParamWriter::fillEngine(tx_car::car &m_car) {
  auto proplusion = m_car.mutable_propulsion();
  auto engine = proplusion->mutable_engine_parameter();

  tx_car::setRealScalar(engine->mutable_tmcst(), "engine torque response time const",
                        "engine output torque will reach 95% of target torque at 3rd  tmCst", "s", 0.05);

  tx_car::setRealScalar(engine->mutable_maxspd(), "engine maximum speed",
                        "normally gasoline is 6000rpm diesel is 3000rpm", "rpm", 6100.0);

  tx_car::setRealScalar(engine->mutable_fueldensity(), "fuel density", "", "kg/L", 0.758);

  auto engineTrqMap = engine->mutable_engine_outtrq_map();

  double engTorqueCoeff = 180 / 514.89;

  // EngTrq output torque Table
  double f_tbrake[] = {
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       41.13,   126.46,  168.44,  171.86,
      173.81,  174.77,  175.1,   175.18,  175.23,  175.18,  49.35,   154.32,  232.06,  247.82,  250.35,  251.38,
      251.74,  251.77,  251.77,  251.77,  0,       145.21,  245.96,  268.1,   274.08,  276.25,  277.05,  277.12,
      277.1,   277.07,  -16.45,  117.35,  288.44,  330.1,   346.63,  352.29,  354.4,   354.59,  354.55,  354.4,
      -24.68,  78.29,   278.01,  345.45,  367.46,  377.44,  382.32,  383.45,  384.2,   391.51,  -27.97,  42.77,
      246.75,  351.59,  388.29,  402.59,  410.23,  412.31,  413.85,  424.41,  -33.51,  16.45,   195.49,  338.42,
      394.8,   427.7,   432.64,  437.57,  442.51,  453.42,  -42.07,  -10.64,  135.08,  302.54,  398.09,  443.64,
      455.67,  463.99,  468.83,  477.05,  -50.08,  -25.19,  93.73,   255.06,  387.36,  448.48,  473.93,  480.34,
      490.21,  498.44,  -57.14,  -36.65,  62.58,   203.53,  371.29,  444.15,  477.05,  490.21,  500.08,  509.95,
      -64.08,  -46.7,   39.02,   160.4,   334.05,  430.99,  478.7,   493.5,   505.02,  514.89,  -71.06,  -56.01,
      19.61,   126.98,  289.7,   413.12,  473.76,  491.86,  503.37,  509.95,  -78.44,  -65.43,  0.46,    95.46,
      249.64,  394.29,  465.54,  481.99,  493.5,   503.27,  -86.11,  -74.79,  -16.57,  68.44,   207.81,  366.08,
      452.38,  468.83,  485.28,  493.5,   -94.28,  -84.03,  -31.71,  44.85,   170.57,  330.59,  435.93,  452.38,
      468.83,  477.05,  -102.47, -93.46,  -45.75,  24,      138.78,  293.42,  410.15,  426.06,  448.04,  452.77,
      -111.65, -103.3,  -59.35,  4.69,    109.99,  254.69,  367.91,  393,     404.78,  409.44,  -121.65, -113.67,
      -72.91,  -13.57,  83.71,   217.11,  322.13,  345.48,  356.46,  360.66,  -133.25, -133.25, -133.25, -133.25,
      -133.25, -133.25, -133.25, -133.25, -133.25, -133.25, -134.89, -134.89, -134.89, -134.89, -134.89, -134.89,
      -134.89, -134.89, -134.89, -134.89};
  // engine torque for fuel consumaption
  double f_tbrake_t_bpt[] = {0.0,     51.489000000000004, 77.233499999999992, 102.97800000000001, 180.21149999999997,
                             257.445, 360.42299999999994, 437.6565,           489.14549999999997, 514.89};  // 10

  // engine throttle
  double f_tbrake_throttle_bpt[] = {0.0, 0.1, 0.15, 0.2, 0.35, 0.5, 0.7, 0.85, 0.95, 1};

  // engine spd rpm
  double f_tbrake_n_bpt[] = {0.0,    400.0,  517.0,  633.0,  750.0,  1107.0, 1463.0, 1820.0,
                             2177.0, 2533.0, 2890.0, 3247.0, 3603.0, 3960.0, 4317.0, 4673.0,
                             5030.0, 5387.0, 5743.0, 6100.0, 6500.0, 6700.0};  // 22

  engineTrqMap->set_comment("engine output torque table with engine spd and engine torque reqired");
  engineTrqMap->set_disp_name("engine output torque table");
  auto y0Axis = engineTrqMap->mutable_y0_axis();
  y0Axis->set_unit("Nm");
  y0Axis->set_disp_name("output torque");
  y0Axis->set_comment(
      "engine static output torque[Nm] as engine speed and torque require "
      "changed!"
      "engine output torque points number should strictly equal torque "
      "required number multiply engine spd number!!!");
  for (int i = 0; i < 10 * 22; i++) {
    y0Axis->add_data(f_tbrake[i] * engTorqueCoeff);
  }

  auto u0Axis = engineTrqMap->mutable_u0_axis();
  tx_car::setRealAxis(u0Axis, "throttle input[0-1]", "input throttle value, range [0,1]", "N/A", f_tbrake_throttle_bpt,
                      10);

  auto u1Axis = engineTrqMap->mutable_u1_axis();
  tx_car::setRealAxis(u1Axis, "engine rotation speed", "engine rotation speed", "rpm", f_tbrake_n_bpt, 22);

  // Engine fuel rate Table
  auto engineFuelRateMap = engine->mutable_engine_fuelrate_map();

  double f_fuel[] = {
      0.0,         0.000263472, 0.000395209, 0.000432848, 0.000619882, 0.00070928,  0.000792613, 0.000896407,
      0.001031162, 0.001085124, 0.0,         0.000263472, 0.000395209, 0.000432848, 0.000619882, 0.00070928,
      0.000792613, 0.000896407, 0.001031162, 0.001085124, 0.0,         0.000263472, 0.000395209, 0.000432848,
      0.000619882, 0.00070928,  0.000792613, 0.000896407, 0.001031162, 0.001085124, 0.0,         0.000309147,
      0.00046372,  0.000511691, 0.000763775, 0.001207456, 0.001343539, 0.001511661, 0.001813993, 0.001909467,
      0.0,         0.000351636, 0.000527454, 0.00058606,  0.000879091, 0.001417888, 0.001689349, 0.001892549,
      0.002365515, 0.002489747, 0.0,         0.000499053, 0.00074858,  0.000837121, 0.001261495, 0.002027403,
      0.00307965,  0.003436835, 0.00425465,  0.004700518, 0.0,         0.000635116, 0.000952674, 0.001071758,
      0.00162212,  0.002598201, 0.00400123,  0.004729849, 0.005816169, 0.006862645, 0.0,         0.000761879,
      0.001142818, 0.001292885, 0.001964845, 0.003137147, 0.004817039, 0.006100918, 0.007454946, 0.009254679,
      0.0,         0.000879899, 0.001319849, 0.001501004, 0.002289994, 0.003645296, 0.00558186,  0.007477664,
      0.009137518, 0.011826697, 0.0,         0.000989661, 0.001484491, 0.001696561, 0.00259786,  0.004123587,
      0.006297841, 0.008412117, 0.010813556, 0.013753938, 0.0,         0.001168079, 0.001752119, 0.001992606,
      0.003040001, 0.004839185, 0.007410002, 0.009928673, 0.013408575, 0.017258979, 0.0,         0.001359242,
      0.002038863, 0.002306592, 0.003505413, 0.005596878, 0.008593915, 0.011553555, 0.015719926, 0.019838748,
      0.0,         0.00156413,  0.002346195, 0.00263947,  0.003994873, 0.006398715, 0.009854021, 0.013295108,
      0.018236338, 0.022227115, 0.0,         0.00178523,  0.002677846, 0.00299458,  0.004512666, 0.007252498,
      0.01120386,  0.015174458, 0.02099771,  0.024195085, 0.0,         0.002024018, 0.003036028, 0.003373364,
      0.005060046, 0.008161365, 0.012650116, 0.017204157, 0.02403522,  0.026030797, 0.0,         0.002190929,
      0.003286393, 0.003777463, 0.005638419, 0.009128869, 0.014200463, 0.019398846, 0.025811673, 0.027630409,
      0.0,         0.00245657,  0.003684855, 0.004211263, 0.006253088, 0.010165118, 0.015873223, 0.02178871,
      0.026841783, 0.028749895, 0.0,         0.002745311, 0.004117967, 0.004677197, 0.006906173, 0.011275385,
      0.017679804, 0.023749422, 0.027472093, 0.029223282, 0.0,         0.002926735, 0.004390103, 0.00517807,
      0.00760007,  0.012332324, 0.019633515, 0.024466399, 0.026459757, 0.028173024, 0.0,         0.003249972,
      0.004874958, 0.005499952, 0.008341594, 0.011595628, 0.019448726, 0.023987265, 0.024749625, 0.026359198,
      0.0,         0.0,         0.0,         0.0,         0.0,         0.0,         0.0,         0.0,
      0.0,         0.0,         0.0,         0.0,         0.0,         0.0,         0.0,         0.0,
      0.0,         0.0,         0.0,         0.0};
  // engine fuel rate kg/s

  engineFuelRateMap->set_comment("Engine fuel rate test data with engine torque and engine spd ");
  engineFuelRateMap->set_disp_name("engine fuel rate table");
  y0Axis = engineFuelRateMap->mutable_y0_axis();
  y0Axis->set_unit("kg/s");
  y0Axis->set_disp_name("engine fuel rate");
  y0Axis->set_comment(
      "Engine fuel rate[kg/s] as engine speed and torque require changed!"
      "engine output torque points number should strictly equal torque "
      "required number multiply engine spd number!!!");
  for (int i = 0; i < 10 * 22; i++) {
    y0Axis->add_data(f_fuel[i] * engTorqueCoeff);
  }

  u0Axis = engineFuelRateMap->mutable_u0_axis();
  for (auto i = 0; i < 10; ++i) {
    f_tbrake_t_bpt[i] = f_tbrake_t_bpt[i] * engTorqueCoeff;
  }
  tx_car::setRealAxis(u0Axis, "torque required",
                      "engine torque number and the value should be strictly the same as "
                      "engine torque of engine output torque map!!!!",
                      "Nm", f_tbrake_t_bpt, 10);

  u1Axis = engineFuelRateMap->mutable_u1_axis();
  tx_car::setRealAxis(u1Axis, "engine rotation speed",
                      "engine rotation speed and the value should be strictly the same as "
                      "engine speed of engine output torque map!!!!",
                      "rpm", f_tbrake_n_bpt, 22);
}

void PowerParamWriter::fillFrontMotor(tx_car::car &m_car) {
  auto proplusion = m_car.mutable_propulsion();
  auto motor = proplusion->mutable_front_motor_parameter();

  tx_car::setRealScalar(motor->mutable_mot_tc_s(), "Motor output torque time constant",
                        "Motor output torque time constant, tau_s [s]", "s", 0.05);

  /* Expression: T_t_extended
   * Referenced by: '<S6>/Max Torque Limit'
   */
  double T_t_extended[] = {3.80640e+02,   3.80640e+02,   3.80640e+02,  3.546446e+02,  2.90180e+02,
                           2.5382650e+02, 2.4031340e+02, 2.038175e+02, 1.7650110e+02, 1.554483e+02,
                           1.3878670e+02, 0.0000e+00,    0.0000e+00};  // trq for 150 kW
  /* Expression: w_t_extended
   * Referenced by: '<S6>/Max Torque Limit'
   */
  double w_t_extended[] = {0,           174.56782880, 349.0309379, 523.59876670, 698.1665955, 819.64150930,
                           872.6297045, 1047.1975330, 1221.765362, 1396.2284710, 1570.7963,   1649.3361150,
                           2356.194450};  // spd rad/s

  tx_car::set1DMapInfo(motor->mutable_mot_max_tor_map(), "Motor torque limit table",
                       "Motor maximum output torque change with speed");
  tx_car::setRealAxis(motor->mutable_mot_max_tor_map()->mutable_u0_axis(), "motor speed",
                      "motor rotation speed in rpm, range [0,inf], motor "
                      "module will handle reverse rotate situation.",
                      "rad/s", w_t_extended, 13);
  tx_car::setRealAxis(motor->mutable_mot_max_tor_map()->mutable_y0_axis(), "motor limit torque",
                      "motor maximum output torque change with rotation speed", "Nm", T_t_extended, 13);

  // loss W  每一行相同转速是不同扭矩下的损失 [-380,380Nm]
  double uDLookupTable_tableData[] = {
      2541.4695,          2329.680375,        2117.89125,         1906.1021250000001, 1694.3130000000003,
      1482.5238749999999, 1270.73475,         1058.945625,        847.15650000000016, 635.367375,
      423.57824999999991, 211.7891250000001,  105.89456249999991, 211.7891250000001,  423.57824999999991,
      635.367375,         847.15650000000016, 1058.945625,        1270.73475,         1482.5238749999999,
      1694.3130000000003, 1906.1021250000001, 2117.89125,         2329.680375,        2541.4695,
      2772.5121818181819, 2541.4694999999997, 2310.4268181818184, 2079.3841363636361, 1848.3414545454548,
      1617.2987727272725, 1386.256090909091,  1155.2134090909092, 924.17072727272739, 693.12804545454549,
      462.08536363636352, 231.0426818181819,  115.5213409090908,  231.0426818181819,  462.08536363636352,
      693.12804545454549, 924.17072727272739, 1155.2134090909092, 1386.256090909091,  1617.2987727272725,
      1848.3414545454548, 2079.3841363636361, 2310.4268181818184, 2541.4694999999997, 2772.5121818181819,
      3003.5548636363642, 2753.258625,        2502.9623863636366, 2252.6661477272728, 2002.3699090909097,
      1752.0736704545454, 1501.7774318181821, 1251.4811931818183, 1001.1849545454548, 750.888715909091,
      500.59247727272719, 250.29623863636377, 125.14811931818171, 250.29623863636377, 500.59247727272719,
      750.888715909091,   1001.1849545454548, 1251.4811931818183, 1501.7774318181821, 1752.0736704545454,
      2002.3699090909097, 2252.6661477272728, 2502.9623863636366, 2753.258625,        3003.5548636363642,
      3234.5975454545455, 2965.0477499999997, 2695.4979545454548, 2425.948159090909,  2156.3983636363641,
      1886.8485681818179, 1617.2987727272728, 1347.7489772727274, 1078.1991818181821, 808.64938636363638,
      539.09959090909081, 269.54979545454557, 134.77489772727262, 269.54979545454557, 539.09959090909081,
      808.64938636363638, 1078.1991818181821, 1347.7489772727274, 1617.2987727272728, 1886.8485681818179,
      2156.3983636363641, 2425.948159090909,  2695.4979545454548, 2965.0477499999997, 3234.5975454545455,
      3465.6402272727278, 3176.836875,        2888.0335227272731, 2599.2301704545457, 2310.4268181818188,
      2021.623465909091,  1732.8201136363639, 1444.0167613636365, 1155.2134090909094, 866.41005681818194,
      577.60670454545448, 288.80335227272741, 144.40167613636353, 288.80335227272741, 577.60670454545448,
      866.41005681818194, 1155.2134090909094, 1444.0167613636365, 1732.8201136363639, 2021.623465909091,
      2310.4268181818188, 2599.2301704545457, 2888.0335227272731, 3176.836875,        3465.6402272727278,
      3696.6829090909096, 3388.6259999999997, 3080.5690909090913, 2772.5121818181819, 2464.4552727272735,
      2156.3983636363637, 1848.3414545454548, 1540.2845454545457, 1232.2276363636367, 924.17072727272739,
      616.113818181818,   308.05690909090924, 154.02845454545442, 308.05690909090924, 616.113818181818,
      924.17072727272739, 1232.2276363636367, 1540.2845454545457, 1848.3414545454548, 2156.3983636363637,
      2464.4552727272735, 2772.5121818181819, 3080.5690909090913, 3388.6259999999997, 3696.6829090909096,
      3927.7255909090909, 3600.415125,        3273.1046590909091, 2945.7941931818182, 2618.4837272727277,
      2291.1732613636359, 1963.8627954545454, 1636.5523295454545, 1309.2418636363639, 981.93139772727272,
      654.6209318181817,  327.31046590909108, 163.65523295454531, 327.31046590909108, 654.6209318181817,
      981.93139772727272, 1309.2418636363639, 1636.5523295454545, 1963.8627954545454, 2291.1732613636359,
      2618.4837272727277, 2945.7941931818182, 3273.1046590909091, 3600.415125,        3927.7255909090909,
      4158.7682727272722, 3812.20425,         3465.6402272727273, 3119.0762045454544, 2772.5121818181819,
      2425.9481590909086, 2079.3841363636361, 1732.8201136363637, 1386.256090909091,  1039.6920681818181,
      693.12804545454526, 346.56402272727286, 173.2820113636362,  346.56402272727286, 693.12804545454526,
      1039.6920681818181, 1386.256090909091,  1732.8201136363637, 2079.3841363636361, 2425.9481590909086,
      2772.5121818181819, 3119.0762045454544, 3465.6402272727273, 3812.20425,         4158.7682727272722,
      4389.8109545454545, 4023.993375,        3658.1757954545456, 3292.3582159090911, 2926.5406363636371,
      2560.7230568181817, 2194.9054772727272, 1829.0878977272728, 1463.2703181818185, 1097.4527386363636,
      731.63515909090893, 365.81757954545469, 182.90878977272712, 365.81757954545469, 731.63515909090893,
      1097.4527386363636, 1463.2703181818185, 1829.0878977272728, 2194.9054772727272, 2560.7230568181817,
      2926.5406363636371, 3292.3582159090911, 3658.1757954545456, 4023.993375,        4389.8109545454545,
      4620.8536363636367, 4235.7824999999993, 3850.7113636363638, 3465.6402272727273, 3080.5690909090913,
      2695.4979545454544, 2310.4268181818184, 1925.3556818181819, 1540.2845454545457, 1155.2134090909092,
      770.1422727272726,  385.07113636363653, 192.535568181818,   385.07113636363653, 770.1422727272726,
      1155.2134090909092, 1540.2845454545457, 1925.3556818181819, 2310.4268181818184, 2695.4979545454544,
      3080.5690909090913, 3465.6402272727273, 3850.7113636363638, 4235.7824999999993, 4620.8536363636367,
      4851.896318181819,  4447.571625,        4043.246931818182,  3638.922238636364,  3234.5975454545464,
      2830.2728522727271, 2425.9481590909095, 2021.623465909091,  1617.2987727272732, 1212.9740795454547,
      808.64938636363627, 404.32469318181836, 202.16234659090892, 404.32469318181836, 808.64938636363627,
      1212.9740795454547, 1617.2987727272732, 2021.623465909091,  2425.9481590909095, 2830.2728522727271,
      3234.5975454545464, 3638.922238636364,  4043.246931818182,  4447.571625,        4851.896318181819,
      5082.939,           4659.36075,         4235.7825,          3812.2042500000002, 3388.6260000000007,
      2965.0477499999997, 2541.4695,          2117.89125,         1694.3130000000003, 1270.73475,
      847.15649999999982, 423.5782500000002,  211.78912499999981, 423.5782500000002,  847.15649999999982,
      1270.73475,         1694.3130000000003, 2117.89125,         2541.4695,          2965.0477499999997,
      3388.6260000000007, 3812.2042500000002, 4235.7825,          4659.36075,         5082.939};
  double uDLookupTable_bp01Data[] = {-2356.19445,  -1649.336115, -1570.7963,   -1396.228471, -1221.765362, -1047.197533,
                                     -872.6297045, -819.6415093, -698.1665955, -523.5987667, -349.0309379, -174.5678288,
                                     0.0,          174.5678288,  349.0309379,  523.5987667,  698.1665955,  819.6415093,
                                     872.6297045,  1047.197533,  1221.765362,  1396.228471,  1570.7963,    1649.336115,
                                     2356.19445};  // spd rad/s  25p
  double uDLookupTable_bp02Data[] = {0.0,      138.7867, 155.4483, 176.5011, 203.8175, 240.3134,
                                     253.8265, 290.18,   354.6446, 380.641,  380.642,  380.643};  // torque Nm 12p

  double motorLoss_All[25 * 23];
  auto index = 0;
  for (auto col = 11; col > 0; col--) {
    for (auto r = 0; r < 25; ++r) {
      motorLoss_All[index] = uDLookupTable_tableData[col * 25 + r];
      index++;
    }
  }

  for (auto i = 275; i < 575; ++i) {
    motorLoss_All[i] = uDLookupTable_tableData[i - 275];
  }

  double bp2_All[23];
  for (auto i = 0; i < 11; i++) {
    bp2_All[i] = -uDLookupTable_bp02Data[11 - i];
  }
  for (auto i = 11; i < 23; ++i) {
    bp2_All[i] = uDLookupTable_bp02Data[i - 11];
  }

  auto mot_loss_map_spd_tor = motor->mutable_mot_loss_map_spd_tor();

  mot_loss_map_spd_tor->set_comment("motor loss[W] table with motor spd and motor output torque");
  mot_loss_map_spd_tor->set_disp_name("motor loss[W] table");
  auto y0Axis = mot_loss_map_spd_tor->mutable_y0_axis();
  y0Axis->set_unit("w");
  y0Axis->set_disp_name("motor loss");
  y0Axis->set_comment("motor loss change with motor speed and motor torque!");
  for (int i = 0; i < 575; i++) {
    y0Axis->add_data(motorLoss_All[i]);
  }

  auto u0Axis = mot_loss_map_spd_tor->mutable_u0_axis();
  tx_car::setRealAxis(u0Axis, "motor ration speed", "", "rad/s", uDLookupTable_bp01Data, 25);

  auto u1Axis = mot_loss_map_spd_tor->mutable_u1_axis();
  tx_car::setRealAxis(u1Axis, "motor output torque", "", "Nm", bp2_All, 23);
}

void PowerParamWriter::fillRearMotor(tx_car::car &m_car) {
  auto proplusion = m_car.mutable_propulsion();
  auto motor = proplusion->mutable_rear_motor_parameter();

  tx_car::setRealScalar(motor->mutable_mot_tc_s(), "Motor output torque time constant",
                        "Motor output torque time constant, tau_s [s]", "s", 0.05);

  /* Expression: T_t_extended
   * Referenced by: '<S6>/Max Torque Limit'
   */
  double T_t_extended[] = {3.8064e+02,    3.806400e+02, 3.8064e+02,    3.546446e+02, 2.9018e+02,
                           2.538265e+02,  2.403134e+02, 2.0381750e+02, 1.765011e+02, 1.5544830e+02,
                           1.3878670e+02, 0.0000e+00,   0.0000e+00};

  /* Expression: w_t_extended
   * Referenced by: '<S6>/Max Torque Limit'
   */
  double w_t_extended[] = {0.0,         174.5678288, 349.0309379, 523.59876670, 698.1665955, 819.6415093, 872.62970450,
                           1047.197533, 1221.765362, 1396.228471, 1570.79630,   1649.336115, 2356.19445};

  tx_car::set1DMapInfo(motor->mutable_mot_max_tor_map(), "Motor torque limit table",
                       "Motor maximum output torque change with speed");
  tx_car::setRealAxis(motor->mutable_mot_max_tor_map()->mutable_u0_axis(), "motor speed",
                      "motor rotation speed in rpm, range [0,inf], motor "
                      "module will handle reverse rotate situation.",
                      "rad/s", w_t_extended, 13);
  tx_car::setRealAxis(motor->mutable_mot_max_tor_map()->mutable_y0_axis(), "motor limit torque",
                      "motor maximum output torque with respect to rotation speed", "Nm", T_t_extended, 13);

  double uDLookupTable_tableData[] = {
      2541.46950,          2329.680375,         2117.89125,          1906.1021250000001,
      1694.3130000000003,  1482.5238749999999,  1270.73475,          1058.945625,
      847.15650000000016,  635.367375,          423.57824999999991,  211.7891250000001,
      105.89456249999991,  211.7891250000001,   423.57824999999991,  635.367375,
      847.15650000000016,  1058.945625,         1270.73475,          1482.52387499999990,
      1694.3130000000003,  1906.1021250000001,  2117.89125,          2329.680375,
      2541.4695,           2772.5121818181819,  2541.4694999999997,  2310.42681818181840,
      2079.3841363636361,  1848.3414545454548,  1617.2987727272725,  1386.256090909091,
      1155.2134090909092,  924.17072727272739,  693.12804545454549,  462.08536363636352,
      231.0426818181819,   115.5213409090908,   231.0426818181819,   462.08536363636352,
      693.128045454545490, 924.17072727272739,  1155.2134090909092,  1386.256090909091,
      1617.2987727272725,  1848.3414545454548,  2079.3841363636361,  2310.4268181818184,
      2541.4694999999997,  2772.5121818181819,  3003.5548636363642,  2753.258625,
      2502.9623863636366,  2252.66614772727280, 2002.3699090909097,  1752.0736704545454,
      1501.7774318181821,  1251.4811931818183,  1001.1849545454548,  750.888715909091,
      500.59247727272719,  250.29623863636377,  125.14811931818171,  250.296238636363770,
      500.59247727272719,  750.888715909091,    1001.1849545454548,  1251.4811931818183,
      1501.7774318181821,  1752.0736704545454,  2002.3699090909097,  2252.6661477272728,
      2502.9623863636366,  2753.258625,         3003.5548636363642,  3234.5975454545455,
      2965.0477499999997,  2695.4979545454548,  2425.948159090909,   2156.3983636363641,
      1886.8485681818179,  1617.2987727272728,  1347.7489772727274,  1078.1991818181821,
      808.64938636363638,  539.09959090909081,  269.54979545454557,  134.77489772727262,
      269.54979545454557,  539.09959090909081,  808.64938636363638,  1078.1991818181821,
      1347.7489772727274,  1617.2987727272728,  1886.8485681818179,  2156.3983636363641,
      2425.948159090909,   2695.4979545454548,  2965.0477499999997,  3234.5975454545455,
      3465.6402272727278,  3176.836875,         2888.0335227272731,  2599.2301704545457,
      2310.4268181818188,  2021.6234659090910,  1732.8201136363639,  1444.0167613636365,
      1155.2134090909094,  866.41005681818194,  577.60670454545448,  288.80335227272741,
      144.40167613636353,  288.80335227272741,  577.60670454545448,  866.41005681818194,
      1155.2134090909094,  1444.0167613636365,  1732.8201136363639,  2021.623465909091,
      2310.4268181818188,  2599.2301704545457,  2888.0335227272731,  3176.836875,
      3465.6402272727278,  3696.6829090909096,  3388.6259999999997,  3080.5690909090913,
      2772.5121818181819,  2464.4552727272735,  2156.3983636363637,  1848.3414545454548,
      1540.2845454545457,  1232.2276363636367,  924.17072727272739,  616.113818181818,
      308.05690909090924,  154.02845454545442,  308.05690909090924,  616.113818181818,
      924.17072727272739,  1232.2276363636367,  1540.2845454545457,  1848.3414545454548,
      2156.3983636363637,  2464.4552727272735,  2772.5121818181819,  3080.5690909090913,
      3388.6259999999997,  3696.6829090909096,  3927.7255909090909,  3600.415125,
      3273.1046590909091,  2945.7941931818182,  2618.4837272727277,  2291.1732613636359,
      1963.8627954545454,  1636.5523295454545,  1309.2418636363639,  981.93139772727272,
      654.6209318181817,   327.31046590909108,  163.65523295454531,  327.31046590909108,
      654.6209318181817,   981.93139772727272,  1309.2418636363639,  1636.5523295454545,
      1963.8627954545454,  2291.17326136363590, 2618.4837272727277,  2945.7941931818182,
      3273.1046590909091,  3600.415125,         3927.7255909090909,  4158.7682727272722,
      3812.20425,          3465.6402272727273,  3119.0762045454544,  2772.5121818181819,
      2425.9481590909086,  2079.3841363636361,  1732.8201136363637,  1386.256090909091,
      1039.6920681818181,  693.12804545454526,  346.56402272727286,  173.2820113636362,
      346.56402272727286,  693.12804545454526,  1039.6920681818181,  1386.256090909091,
      1732.8201136363637,  2079.3841363636361,  2425.9481590909086,  2772.5121818181819,
      3119.0762045454544,  3465.6402272727273,  3812.20425,          4158.7682727272722,
      4389.8109545454545,  4023.993375,         3658.1757954545456,  3292.3582159090911,
      2926.5406363636371,  2560.7230568181817,  2194.90547727272720, 1829.0878977272728,
      1463.2703181818185,  1097.4527386363636,  731.63515909090893,  365.81757954545469,
      182.90878977272712,  365.81757954545469,  731.63515909090893,  1097.4527386363636,
      1463.2703181818185,  1829.0878977272728,  2194.90547727272720, 2560.7230568181817,
      2926.5406363636371,  3292.3582159090911,  3658.1757954545456,  4023.993375,
      4389.8109545454545,  4620.8536363636367,  4235.7824999999993,  3850.7113636363638,
      3465.6402272727273,  3080.5690909090913,  2695.4979545454544,  2310.42681818181840,
      1925.3556818181819,  1540.2845454545457,  1155.2134090909092,  770.1422727272726,
      385.07113636363653,  192.535568181818,    385.07113636363653,  770.1422727272726,
      1155.2134090909092,  1540.2845454545457,  1925.3556818181819,  2310.4268181818184,
      2695.4979545454544,  3080.5690909090913,  3465.6402272727273,  3850.7113636363638,
      4235.7824999999993,  4620.8536363636367,  4851.896318181819,   4447.571625,
      4043.246931818182,   3638.922238636364,   3234.5975454545464,  2830.2728522727271,
      2425.9481590909095,  2021.623465909091,   1617.2987727272732,  1212.9740795454547,
      808.64938636363627,  404.32469318181836,  202.16234659090892,  404.32469318181836,
      808.64938636363627,  1212.9740795454547,  1617.2987727272732,  2021.623465909091,
      2425.9481590909095,  2830.2728522727271,  3234.5975454545464,  3638.922238636364,
      4043.246931818182,   4447.571625,         4851.896318181819,   5082.939,
      4659.36075,          4235.7825,           3812.2042500000002,  3388.6260000000007,
      2965.0477499999997,  2541.4695,           2117.89125,          1694.3130000000003,
      1270.73475,          847.15649999999982,  423.5782500000002,   211.78912499999981,
      423.5782500000002,   847.15649999999982,  1270.73475,          1694.3130000000003,
      2117.89125,          2541.4695,           2965.0477499999997,  3388.62600000000070,
      3812.2042500000002,  4235.7825,           4659.36075,          5082.939};  // loss W
  double uDLookupTable_bp01Data[] = {-2356.19445,  -1649.336115, -1570.7963,   -1396.228471, -1221.765362,
                                     -1047.197533, -872.6297045, -819.6415093, -698.1665955, -523.5987667,
                                     -349.0309379, -174.5678288, 0.0,          174.5678288,  349.0309379,
                                     523.5987667,  698.1665955,  819.6415093,  872.6297045,  1047.197533,
                                     1221.765362,  1396.228471,  1570.7963,    1649.336115,  2356.19445};  // spd rad/s
  double uDLookupTable_bp02Data[] = {0.0,      138.7867, 155.4483, 176.5011, 203.8175, 240.3134,
                                     253.8265, 290.18,   354.6446, 380.641,  380.642,  380.643};  // torque Nm

  double motorLoss_All[25 * 23];
  auto index = 0;
  for (auto col = 11; col > 0; col--) {
    for (auto r = 0; r < 25; ++r) {
      motorLoss_All[index] = uDLookupTable_tableData[col * 25 + r];
      index++;
    }
  }

  for (auto i = 275; i < 575; ++i) {
    motorLoss_All[i] = uDLookupTable_tableData[i - 275];
  }

  double bp2_All[23];
  for (auto i = 0; i < 11; i++) {
    bp2_All[i] = -uDLookupTable_bp02Data[11 - i];
  }
  for (auto i = 11; i < 23; ++i) {
    bp2_All[i] = uDLookupTable_bp02Data[i - 11];
  }

  auto mot_loss_map_spd_tor = motor->mutable_mot_loss_map_spd_tor();

  mot_loss_map_spd_tor->set_comment("motor loss[W] table with motor spd and motor output torque");
  mot_loss_map_spd_tor->set_disp_name("motor loss[W] table");
  auto y0Axis = mot_loss_map_spd_tor->mutable_y0_axis();
  y0Axis->set_unit("w");
  y0Axis->set_disp_name("motor loss");
  y0Axis->set_comment("motor loss change with motor speed and motor torque!");
  for (int i = 0; i < 575; i++) {
    y0Axis->add_data(motorLoss_All[i]);
  }

  auto u0Axis = mot_loss_map_spd_tor->mutable_u0_axis();
  tx_car::setRealAxis(u0Axis, "motor ration speed", "", "rad/s", uDLookupTable_bp01Data, 25);

  auto u1Axis = mot_loss_map_spd_tor->mutable_u1_axis();
  tx_car::setRealAxis(u1Axis, "motor output torque", "", "Nm", bp2_All, 23);
}

void PowerParamWriter::fillHybrid(tx_car::car &m_car) {
  auto &hybrid = *m_car.mutable_propulsion()->mutable_hybrid_parameter();
  auto &generator = *hybrid.mutable_motor_generator();
  const auto &rearMotor = *m_car.mutable_propulsion()->mutable_rear_motor_parameter();

  // copy from rear drive motor
  generator.CopyFrom(rearMotor);

  // half performance of rear drive motor
  auto &motorTrqMap = *generator.mutable_mot_max_tor_map();
  for (auto i = 0; i < motorTrqMap.y0_axis().data_size(); ++i) {
    motorTrqMap.mutable_u0_axis()->set_data(i, motorTrqMap.u0_axis().data(i) * 0.5);
    motorTrqMap.mutable_y0_axis()->set_data(i, motorTrqMap.y0_axis().data(i) * 0.5);
  }

  // half performance of rear drive motor
  auto &motorLossPwr = *generator.mutable_mot_loss_map_spd_tor();
  for (auto i = 0; i < motorLossPwr.u0_axis().data_size(); ++i) {
    motorLossPwr.mutable_u0_axis()->set_data(i, motorLossPwr.u0_axis().data(i) * 0.5);
  }
  for (auto i = 0; i < motorLossPwr.u1_axis().data_size(); ++i) {
    motorLossPwr.mutable_u1_axis()->set_data(i, motorLossPwr.u1_axis().data(i) * 0.5);
  }
  for (auto i = 0; i < motorLossPwr.y0_axis().data_size(); ++i) {
    motorLossPwr.mutable_y0_axis()->set_data(i, motorLossPwr.y0_axis().data(i) * 0.5);
  }

  tx_car::setRealScalar(hybrid.mutable_generator_gear_ratio(), "gear ratio from engine to generator", "", "N/A", 0.8);
  tx_car::setRealScalar(hybrid.mutable_t_clutch(), "engine clutch time const", "", "s", 0.1);
}

void PowerParamWriter::fillBattery(tx_car::car &m_car) {
  auto proplusion = m_car.mutable_propulsion();
  auto batt = proplusion->mutable_batt_paramter();

  // the left data is for 15kW.h NCA-C battery
  tx_car::setRealScalar(batt->mutable_battchargemax(), "Rated capacity",
                        "battery Rated capacity at nominal temperature, Ah", "Ah", 37.5);
  tx_car::setRealScalar(batt->mutable_battinitsoc(), "battery init soc", "battery init state of charge [0-100]", "%",
                        90.0);
  tx_car::setInt32Scalar(batt->mutable_np(), "parallel number", "Number of cells in parallel", "null", 1);
  tx_car::setInt32Scalar(batt->mutable_ns(), "series number", "Number of cells in series", "null", 99);

  double CapLUTBp[] =  // soc 0-1
      {0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1,  0.11, 0.12, 0.13, 0.14, 0.15, 0.16,
       0.17, 0.18, 0.19, 0.2,  0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.27, 0.28, 0.29, 0.3,  0.31, 0.32, 0.33,
       0.34, 0.35, 0.36, 0.37, 0.38, 0.39, 0.4,  0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.5,
       0.51, 0.52, 0.53, 0.54, 0.55, 0.56, 0.57, 0.58, 0.59, 0.6,  0.61, 0.62, 0.63, 0.64, 0.65, 0.66, 0.67,
       0.68, 0.69, 0.7,  0.71, 0.72, 0.73, 0.74, 0.75, 0.76, 0.77, 0.78, 0.79, 0.8,  0.81, 0.82, 0.83, 0.84,
       0.85, 0.86, 0.87, 0.88, 0.89, 0.9,  0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99, 1.0};

  double Em[] =  // ocv-V
      {2.8,   3.228, 3.284, 3.361, 3.408, 3.427, 3.472, 3.477, 3.493, 3.504, 3.516, 3.528, 3.537, 3.545, 3.555,
       3.561, 3.566, 3.576, 3.587, 3.589, 3.594, 3.6,   3.608, 3.61,  3.616, 3.619, 3.626, 3.632, 3.637, 3.64,
       3.645, 3.646, 3.652, 3.655, 3.658, 3.661, 3.664, 3.668, 3.673, 3.678, 3.68,  3.681, 3.686, 3.692, 3.699,
       3.702, 3.705, 3.71,  3.717, 3.723, 3.728, 3.733, 3.735, 3.742, 3.749, 3.755, 3.761, 3.768, 3.773, 3.78,
       3.791, 3.798, 3.798, 3.814, 3.818, 3.825, 3.841, 3.846, 3.855, 3.863, 3.877, 3.885, 3.894, 3.907, 3.919,
       3.926, 3.935, 3.944, 3.954, 3.964, 3.974, 3.988, 3.998, 4.014, 4.029, 4.034, 4.047, 4.065, 4.074, 4.086,
       4.097, 4.131, 4.126, 4.138, 4.15,  4.18,  4.174, 4.187, 4.207, 4.231, 4.221};

  auto ocvTable = batt->mutable_batt_ocv_soc_table();
  tx_car::set1DMapInfo(ocvTable, "battery open circuit voltage table", "");
  tx_car::setRealAxis(ocvTable->mutable_u0_axis(), "battery soc", "batery state of charge [0-1]", "null", CapLUTBp,
                      101);
  tx_car::setRealAxis(ocvTable->mutable_y0_axis(), "cell open circuit voltage ", "batery cell open circuit voltage",
                      "V", Em, 101);

  // Battery temperature breakpoints, [K]
  double BattTempBp[] = {243.1, 253.1, 263.1, 273.1, 283.1, 298.1, 313.1};
  // Battery capacity breakpoints 2,[0-1]
  double CapSOCBp[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0};
  // Internal resistance table data, [Ohms]
  double RInt[] = {0.008846, 0.006389,  0.004364, 0.002031,  0.001443, 0.0005603, 0.001028,  0.009319, 0.006554,
                   0.004101, 0.002563,  0.001825, 0.0007192, 0.001338, 0.009027,  0.005553,  0.00406,  0.002532,
                   0.001795, 0.0006887, 0.001,    0.00847,   0.005525, 0.003452,  0.00246,   0.001778, 0.0007557,
                   0.001536, 0.01032,   0.006043, 0.003846,  0.002716, 0.001924,  0.0007355, 0.001546, 0.01314,
                   0.007805, 0.004517,  0.003047, 0.002234,  0.001014, 0.002789};

  auto resisTable = batt->mutable_batt_resis_temp_soc();
  resisTable->set_disp_name("Internal resistance table data");
  resisTable->set_comment("");
  tx_car::setRealAxis(resisTable->mutable_y0_axis(), "battery cell resistance", "batery cell resistance", "Ohms", RInt,
                      42);
  tx_car::setRealAxis(resisTable->mutable_u0_axis(), "battery tempreture", "cell tempreture in K", "K", BattTempBp, 7);
  tx_car::setRealAxis(resisTable->mutable_u1_axis(), "battery soc", "batery state of charge [0-1]", "null", CapSOCBp,
                      6);
}
