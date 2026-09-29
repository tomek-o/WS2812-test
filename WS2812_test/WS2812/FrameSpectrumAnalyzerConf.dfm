object fraSpectrumAnalyzerConf: TfraSpectrumAnalyzerConf
  Left = 0
  Top = 0
  Width = 451
  Height = 442
  Align = alClient
  TabOrder = 0
  TabStop = True
  object lblLedsPerBand: TLabel
    Left = 5
    Top = 74
    Width = 68
    Height = 13
    Caption = 'LEDs per band'
  end
  object lblFreqRange: TLabel
    Left = 150
    Top = 74
    Width = 83
    Height = 13
    Caption = 'Frequency range'
  end
  object lblFreqDash: TLabel
    Left = 301
    Top = 74
    Width = 4
    Height = 13
    Caption = '-'
  end
  object lblFreqUnit: TLabel
    Left = 367
    Top = 74
    Width = 12
    Height = 13
    Caption = 'Hz'
  end
  object lblGain: TLabel
    Left = 5
    Top = 97
    Width = 21
    Height = 13
    Caption = 'Gain'
  end
  object lblGainValue: TLabel
    Left = 336
    Top = 129
    Width = 22
    Height = 13
    Caption = '1.0x'
  end
  object lblMaxBrightness: TLabel
    Left = 5
    Top = 167
    Width = 335
    Height = 13
    Caption = 'Max brightness'
  end
  object lblMaxBrightnessValue: TLabel
    Left = 336
    Top = 197
    Width = 12
    Height = 13
    Caption = '20'
  end
  object lblUpdateInterval: TLabel
    Left = 5
    Top = 237
    Width = 420
    Height = 13
    Caption = 'Update interval'
  end
  object lblUpdateIntervalValue: TLabel
    Left = 336
    Top = 267
    Width = 28
    Height = 13
    Caption = '33 ms'
  end
  object lblMeterFloor: TLabel
    Left = 5
    Top = 307
    Width = 329
    Height = 13
    Caption = 'Meter floor'
  end
  object lblMeterFloorValue: TLabel
    Left = 336
    Top = 340
    Width = 31
    Height = 13
    Caption = '-50 dB'
  end
  object lblSmoothing: TLabel
    Left = 5
    Top = 377
    Width = 294
    Height = 13
    Caption = 'Smoothing'
  end
  object lblSmoothingValue: TLabel
    Left = 341
    Top = 408
    Width = 23
    Height = 13
    Caption = '30%'
  end
  object chbAutoStart: TCheckBox
    Left = 5
    Top = 3
    Width = 325
    Height = 17
    Caption = 'Start audio capture automatically at application startup'
    TabOrder = 0
  end
  object chbPeakDetect: TCheckBox
    Left = 5
    Top = 25
    Width = 325
    Height = 17
    Caption = 'Show peak-hold marker (reserves the top LED of each band)'
    TabOrder = 1
  end
  object chbAlternateDirection: TCheckBox
    Left = 5
    Top = 47
    Width = 420
    Height = 17
    Caption =
      'Alternate bar direction on every other band (serpentine-wired LE' +
      'D matrix)'
    TabOrder = 2
  end
  object edLedsPerBand: TEdit
    Left = 80
    Top = 71
    Width = 40
    Height = 21
    TabOrder = 3
    Text = '8'
  end
  object edMinFreq: TEdit
    Left = 240
    Top = 71
    Width = 55
    Height = 21
    TabOrder = 4
    Text = '40'
  end
  object edMaxFreq: TEdit
    Left = 310
    Top = 71
    Width = 55
    Height = 21
    TabOrder = 5
    Text = '16000'
  end
  object trbarGain: TTrackBar
    Left = 40
    Top = 114
    Width = 273
    Height = 45
    Max = 50
    Min = 1
    Frequency = 10
    Position = 10
    TabOrder = 6
    TickMarks = tmBoth
    OnChange = trbarGainChange
  end
  object trbarMaxBrightness: TTrackBar
    Left = 40
    Top = 184
    Width = 273
    Height = 45
    Max = 255
    Min = 2
    Frequency = 25
    Position = 20
    TabOrder = 7
    TickMarks = tmBoth
    OnChange = trbarMaxBrightnessChange
  end
  object trbarUpdateInterval: TTrackBar
    Left = 40
    Top = 254
    Width = 273
    Height = 45
    Max = 100
    Min = 15
    Frequency = 5
    Position = 33
    TabOrder = 8
    TickMarks = tmBoth
    OnChange = trbarUpdateIntervalChange
  end
  object trbarMeterFloor: TTrackBar
    Left = 40
    Top = 324
    Width = 273
    Height = 45
    Min = -80
    Frequency = 10
    Position = -50
    TabOrder = 9
    TickMarks = tmBoth
    OnChange = trbarMeterFloorChange
  end
  object trbarSmoothing: TTrackBar
    Left = 40
    Top = 394
    Width = 273
    Height = 45
    Max = 95
    Frequency = 10
    Position = 30
    TabOrder = 10
    TickMarks = tmBoth
    OnChange = trbarSmoothingChange
  end
end
