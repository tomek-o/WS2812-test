object fraAudioVisualisationConf: TfraAudioVisualisationConf
  Left = 0
  Top = 0
  Width = 451
  Height = 415
  Align = alClient
  TabOrder = 0
  TabStop = True
  ExplicitHeight = 304
  object lblGain: TLabel
    Left = 5
    Top = 70
    Width = 21
    Height = 13
    Caption = 'Gain'
  end
  object lblGainValue: TLabel
    Left = 336
    Top = 102
    Width = 22
    Height = 13
    Caption = '1.0x'
  end
  object lblMaxBrightness: TLabel
    Left = 5
    Top = 140
    Width = 335
    Height = 13
    Caption = 
      'Max brightness (10-255, caps per-channel value to limit current ' +
      'draw)'
  end
  object lblUpdateInterval: TLabel
    Left = 5
    Top = 210
    Width = 420
    Height = 13
    Caption = 
      'Update interval (10-100 ms; actual rate is limited by Windows'#39' t' +
      'imer resolution, ~15 ms)'
  end
  object lblUpdateIntervalValue: TLabel
    Left = 336
    Top = 240
    Width = 28
    Height = 13
    Caption = '33 ms'
  end
  object lblMeterFloor: TLabel
    Left = 5
    Top = 280
    Width = 329
    Height = 13
    Caption = 
      'Meter floor (-80..-10 dB; loudness mapped to the bottom of the b' +
      'ar)'
  end
  object lblMeterFloorValue: TLabel
    Left = 336
    Top = 313
    Width = 31
    Height = 13
    Caption = '-40 dB'
  end
  object lblSmoothing: TLabel
    Left = 5
    Top = 350
    Width = 294
    Height = 13
    Caption = 'Smoothing (0-95%; higher = smoother but slower to rise/fall)'
  end
  object lblSmoothingValue: TLabel
    Left = 341
    Top = 381
    Width = 23
    Height = 13
    Caption = '55%'
  end
  object lblMaxBrightnessValue: TLabel
    Left = 336
    Top = 170
    Width = 12
    Height = 13
    Caption = '20'
  end
  object chbStereoSeparate: TCheckBox
    Left = 5
    Top = 3
    Width = 388
    Height = 17
    Caption = 
      'Separate left/right channels (half the strip each) instead of mi' +
      'xing them'
    TabOrder = 0
  end
  object chbAutoStart: TCheckBox
    Left = 5
    Top = 25
    Width = 325
    Height = 17
    Caption = 'Start audio capture automatically at application startup'
    TabOrder = 1
  end
  object chbPeakDetect: TCheckBox
    Left = 5
    Top = 47
    Width = 325
    Height = 17
    Caption = 'Show peak-hold marker (reserves the top LED for it)'
    TabOrder = 2
  end
  object trbarGain: TTrackBar
    Left = 40
    Top = 87
    Width = 273
    Height = 45
    Max = 50
    Min = 1
    Frequency = 10
    Position = 10
    TabOrder = 3
    TickMarks = tmBoth
    OnChange = trbarGainChange
  end
  object trbarMaxBrightness: TTrackBar
    Left = 40
    Top = 157
    Width = 273
    Height = 45
    Max = 255
    Min = 10
    Frequency = 25
    Position = 255
    TabOrder = 4
    TickMarks = tmBoth
    OnChange = trbarMaxBrightnessChange
  end
  object trbarUpdateInterval: TTrackBar
    Left = 40
    Top = 227
    Width = 273
    Height = 45
    Max = 100
    Min = 15
    Frequency = 5
    Position = 33
    TabOrder = 5
    TickMarks = tmBoth
    OnChange = trbarUpdateIntervalChange
  end
  object trbarMeterFloor: TTrackBar
    Left = 40
    Top = 297
    Width = 273
    Height = 45
    Min = -80
    Frequency = 10
    Position = -40
    TabOrder = 6
    TickMarks = tmBoth
    OnChange = trbarMeterFloorChange
  end
  object trbarSmoothing: TTrackBar
    Left = 40
    Top = 367
    Width = 273
    Height = 45
    Max = 95
    Frequency = 10
    Position = 55
    TabOrder = 7
    TickMarks = tmBoth
    OnChange = trbarSmoothingChange
  end
end
