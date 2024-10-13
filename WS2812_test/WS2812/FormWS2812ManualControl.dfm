object frmWS2812ManualControl: TfrmWS2812ManualControl
  Left = 0
  Top = 0
  Caption = 'Manual control'
  ClientHeight = 238
  ClientWidth = 451
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object pnlBottom: TPanel
    Left = 0
    Top = 173
    Width = 451
    Height = 65
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
    object chbManualControlApplyImmediately: TCheckBox
      Left = 87
      Top = 0
      Width = 234
      Height = 17
      Caption = 'Write immediately after value is changed'
      TabOrder = 1
      OnClick = chbManualControlApplyImmediatelyClick
      OnKeyPress = chbManualControlApplyImmediatelyKeyPress
    end
    object chbWritePeriodically: TCheckBox
      Left = 87
      Top = 16
      Width = 122
      Height = 17
      Caption = 'Write periodically'
      TabOrder = 2
    end
    object btnClearAll: TButton
      Left = 6
      Top = 37
      Width = 75
      Height = 25
      Caption = 'Clear all'
      TabOrder = 3
      OnClick = btnClearAllClick
    end
    object btnClearLed: TButton
      Left = 87
      Top = 37
      Width = 75
      Height = 25
      Caption = 'Clear row'
      TabOrder = 4
      OnClick = btnClearLedClick
    end
  end
  object grid: TStringGrid
    Left = 0
    Top = 0
    Width = 451
    Height = 173
    Align = alClient
    DefaultRowHeight = 20
    TabOrder = 1
    OnDblClick = gridDblClick
    OnDrawCell = gridDrawCell
    OnKeyDown = gridKeyDown
    OnKeyPress = gridKeyPress
    OnSelectCell = gridSelectCell
    ExplicitWidth = 455
    ExplicitHeight = 179
  end
  object tmrWrite: TTimer
    OnTimer = tmrWriteTimer
    Left = 16
    Top = 136
  end
end
