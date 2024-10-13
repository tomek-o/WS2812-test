object frmSettings: TfrmSettings
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Settings'
  ClientHeight = 251
  ClientWidth = 341
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlBottom: TPanel
    Left = 0
    Top = 214
    Width = 341
    Height = 37
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object btnCancel: TButton
      Left = 258
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = btnCancelClick
    end
    object btnApply: TButton
      Left = 177
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Apply'
      TabOrder = 0
      OnClick = btnApplyClick
    end
  end
  object pages: TPageControl
    Left = 0
    Top = 0
    Width = 341
    Height = 214
    ActivePage = tsSerialPort
    Align = alClient
    TabOrder = 0
    object tsGeneral: TTabSheet
      Caption = 'General'
      object chbAlwaysOnTop: TCheckBox
        Left = 3
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Window always on top'
        TabOrder = 0
        OnClick = chbAlwaysOnTopClick
      end
    end
    object tsSerialPort: TTabSheet
      Caption = 'Serial Port / WS2812'
      ImageIndex = 2
      object lblSerialPortName: TLabel
        Left = 3
        Top = 53
        Width = 27
        Height = 13
        Caption = 'Name'
      end
      object lblSerialPortBaudrate: TLabel
        Left = 3
        Top = 76
        Width = 44
        Height = 13
        Caption = 'Baudrate'
      end
      object lblLedCount: TLabel
        Left = 3
        Top = 109
        Width = 48
        Height = 13
        Caption = 'LED count'
      end
      object chbSerialPortOpenAtStartup: TCheckBox
        Left = 3
        Top = 3
        Width = 97
        Height = 17
        Caption = 'Open at startup'
        TabOrder = 0
      end
      object chbSerialPortAutoReinit: TCheckBox
        Left = 3
        Top = 26
        Width = 97
        Height = 17
        Caption = 'Auto reinit'
        TabOrder = 1
      end
      object cbSerialPorts: TComboBox
        Left = 73
        Top = 50
        Width = 97
        Height = 21
        ItemHeight = 13
        TabOrder = 2
      end
      object edSerialPortBaudrate: TEdit
        Left = 73
        Top = 73
        Width = 97
        Height = 21
        TabOrder = 3
      end
      object cbLedCount: TComboBox
        Left = 73
        Top = 106
        Width = 97
        Height = 21
        ItemHeight = 13
        TabOrder = 4
        Items.Strings = (
          '1'
          '2'
          '3'
          '4'
          '8'
          '16'
          '32'
          '64')
      end
    end
    object tsLogging: TTabSheet
      Caption = 'Logging'
      ImageIndex = 1
      object lblUiCapacity: TLabel
        Left = 5
        Top = 26
        Width = 194
        Height = 13
        Caption = 'Visible (buffered) log lines in log window:'
      end
      object cmbMaxUiLogLines: TComboBox
        Left = 216
        Top = 23
        Width = 111
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnChange = cmbMaxUiLogLinesChange
        Items.Strings = (
          '100'
          '200'
          '500'
          '1000'
          '2000'
          '5000'
          '10000')
      end
      object chbLogToFile: TCheckBox
        Left = 5
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Log to file'
        TabOrder = 1
        OnClick = chbAlwaysOnTopClick
      end
    end
  end
end
