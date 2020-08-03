PROGRAM RPi
  VAR
    Motor AT %IX100.0 : BOOL;
    Motor_Critical AT %QX100.0 : BOOL;
    Counter AT %IW100 : UINT;
    Error AT %IW101 : INT;
    Pressure_HIGH AT %IW102 : UINT;
    Pressure_LOW AT %IW103 : UINT;
    Temperature AT %IW104 : INT;
    DWT_CYCCNT_HIGH AT %IW105 : UINT;
    DWT_CYCCNT_LOW AT %IW106 : UINT;
    DWT_FOLDCNT AT %IW107 : UINT;
    DWT_LSUCNT AT %IW108 : UINT;
    DWT_SLEEPCNT AT %IW109 : UINT;
    DWT_EXCCNT AT %IW110 : UINT;
    DWT_CPICNT AT %IW111 : UINT;
    Pressure_Threshold_Upper_Actual AT %IW112 : UINT;
    Pressure_Threshold_Lower_Actual AT %IW113 : UINT;
    Pressure_Threshold_Upper_Set AT %QW100 : UINT;
    Pressure_Threshold_Lower_Set AT %QW101 : UINT;
  END_VAR

  Motor_Critical := NOT GT(Pressure_LOW, 36464);
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK task0(INTERVAL := T#10ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : RPi;
  END_RESOURCE
END_CONFIGURATION
