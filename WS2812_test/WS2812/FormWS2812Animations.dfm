object frmWS2812Animations: TfrmWS2812Animations
  Left = 0
  Top = 0
  Caption = 'Animations'
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
  object lblType: TLabel
    Left = 8
    Top = 19
    Width = 24
    Height = 13
    Caption = 'Type'
  end
  object cbType: TComboBox
    Left = 40
    Top = 16
    Width = 185
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 0
    Text = 'moving pixel'
    Items.Strings = (
      'moving pixel'
      'all random'
      'all random, half off')
  end
  object btnStart: TButton
    Left = 40
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Start'
    TabOrder = 1
    OnClick = btnStartClick
  end
  object btnStop: TButton
    Left = 150
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 2
    OnClick = btnStopClick
  end
  object tmrWrite: TTimer
    Enabled = False
    OnTimer = tmrWriteTimer
    Left = 16
    Top = 144
  end
end
