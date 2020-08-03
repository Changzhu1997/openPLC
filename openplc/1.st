PROGRAM RPi
  VAR
    presure_sensor AT %IW100 : UINT;
    temp_sensor AT %IW101 : INT;
    relay AT %QX100.0 : BOOL;
    counter AT %IW102 : UINT;
    error_code AT %IW103 : INT;
  END_VAR
  VAR
    GT1_OUT : BOOL;
  END_VAR

  GT1_OUT := GT(presure_sensor, 10040);
  relay := GT1_OUT;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : RPi;
  END_RESOURCE
END_CONFIGURATION
