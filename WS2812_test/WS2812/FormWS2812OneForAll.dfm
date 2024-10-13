object frmWS2812OneForAll: TfrmWS2812OneForAll
  Left = 0
  Top = 0
  Caption = 'One for all'
  ClientHeight = 213
  ClientWidth = 455
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblRed: TLabel
    Left = 8
    Top = 37
    Width = 19
    Height = 13
    Caption = 'Red'
  end
  object lblGreen: TLabel
    Left = 8
    Top = 90
    Width = 29
    Height = 13
    Caption = 'Green'
  end
  object lblBlue: TLabel
    Left = 8
    Top = 137
    Width = 20
    Height = 13
    Caption = 'Blue'
  end
  object lblDescription: TLabel
    Left = 8
    Top = 8
    Width = 157
    Height = 13
    Caption = 'Set same RGB values for all LEDs'
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 179
    Width = 455
    Height = 34
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object btnWrite: TButton
      Left = 6
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Write'
      TabOrder = 0
      OnClick = btnWriteClick
    end
    object chbApplyImmediately: TCheckBox
      Left = 87
      Top = 0
      Width = 234
      Height = 17
      Caption = 'Write immediately after value is changed'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object chbWritePeriodically: TCheckBox
      Left = 87
      Top = 16
      Width = 122
      Height = 17
      Caption = 'Write periodically'
      TabOrder = 2
    end
  end
  object trbarRed: TTrackBar
    Left = 48
    Top = 24
    Width = 273
    Height = 45
    Max = 255
    Frequency = 10
    TabOrder = 1
    TickMarks = tmBoth
    OnChange = trbarChange
  end
  object trbarGreen: TTrackBar
    Left = 48
    Top = 75
    Width = 273
    Height = 45
    Max = 255
    Frequency = 10
    TabOrder = 2
    TickMarks = tmBoth
    OnChange = trbarChange
  end
  object trbarBlue: TTrackBar
    Left = 48
    Top = 126
    Width = 273
    Height = 45
    Max = 255
    Frequency = 10
    TabOrder = 3
    TickMarks = tmBoth
    OnChange = trbarChange
  end
  object tmrWrite: TTimer
    OnTimer = tmrWriteTimer
    Left = 344
    Top = 136
  end
end
