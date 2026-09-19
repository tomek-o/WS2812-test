object frmWS2812AudioVisualisation: TfrmWS2812AudioVisualisation
  Left = 0
  Top = 0
  Caption = 'Audio visualisation'
  ClientHeight = 262
  ClientWidth = 508
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    508
    262)
  PixelsPerInch = 96
  TextHeight = 13
  object lblGain: TLabel
    Left = 8
    Top = 80
    Width = 21
    Height = 13
    Caption = 'Gain'
  end
  object lblGainValue: TLabel
    Left = 320
    Top = 78
    Width = 22
    Height = 13
    Caption = '1.0x'
  end
  object lblDevice: TLabel
    Left = 8
    Top = 11
    Width = 32
    Height = 13
    Caption = 'Device'
  end
  object image: TImage
    Left = 8
    Top = 120
    Width = 492
    Height = 134
    Anchors = [akLeft, akTop, akRight, akBottom]
  end
  object cbSource: TComboBox
    Left = 48
    Top = 8
    Width = 120
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 0
    Text = 'Output (loopback)'
    OnChange = cbSourceChange
    Items.Strings = (
      'Output (loopback)'
      'Input (microphone)')
  end
  object cbAudioDevice: TComboBox
    Left = 176
    Top = 8
    Width = 232
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 1
    OnChange = cbAudioDeviceChange
  end
  object btnRefreshDevices: TButton
    Left = 416
    Top = 7
    Width = 71
    Height = 23
    Caption = 'Refresh list'
    TabOrder = 2
    OnClick = btnRefreshDevicesClick
  end
  object btnStart: TButton
    Left = 8
    Top = 35
    Width = 120
    Height = 25
    Caption = 'Start visualisation'
    TabOrder = 3
    OnClick = btnStartClick
  end
  object btnStop: TButton
    Left = 142
    Top = 35
    Width = 120
    Height = 25
    Caption = 'Stop visualisation'
    Enabled = False
    TabOrder = 4
    OnClick = btnStopClick
  end
  object trbarGain: TTrackBar
    Left = 40
    Top = 69
    Width = 273
    Height = 45
    Max = 50
    Min = 1
    Frequency = 10
    Position = 10
    TabOrder = 5
    TickMarks = tmBoth
    OnChange = trbarGainChange
  end
  object tmrWrite: TTimer
    Enabled = False
    Interval = 33
    OnTimer = tmrWriteTimer
    Left = 16
    Top = 144
  end
end
