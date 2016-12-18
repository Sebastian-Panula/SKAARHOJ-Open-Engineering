namespace VMIX {
// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
// Bit 5 (32) = output bit; If this is set, a binary output will be set if coupled with this hwc.
uint16_t evaluateAction(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int16_t pulses, int16_t value, uint8_t HWcType) {
  uint16_t retVal = 0;
  int16_t tempInt = 0;
  uint8_t tempByte = 0;

  if (actDown || actUp) {
    if (debugMode)
      Serial << F("VMIX action ") << globalConfigMem[actionPtr] << F("\n");
  }

  switch (globalConfigMem[actionPtr]) {
  case 0: // Active Source
    if (actDown) {
      if (globalConfigMem[actionPtr + 2] == 3) {       // Source cycle by button push
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used to show button is highlighted here
        pulses = 2;
      } else if (globalConfigMem[actionPtr + 2] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
        _systemHWcActionCache[HWc][actIdx] = VMix[devIndex].getActiveInput();
        VMix[devIndex].setActiveInput(globalConfigMem[actionPtr + 1] - 1);
      } else {
        VMix[devIndex].setActiveInput(_systemHWcActionCache[HWc][actIdx]);
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 1) { // "Hold Down"
      VMix[devIndex].setActiveInput(_systemHWcActionCache[HWc][actIdx]);
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 3) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      VMix[devIndex].setActiveInput(pulsesHelper(VMix[devIndex].getActiveInput(), globalConfigMem[actionPtr + 3] - 1, globalConfigMem[actionPtr + 1] - 1, true, pulses, 1, 1));
    }

    retVal = globalConfigMem[actionPtr + 2] == 3 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (2 | 0x20) : 5) : (VMix[devIndex].getActiveInput() == (globalConfigMem[actionPtr + 1] - 1) ? (2 | 0x20) : 5);

    // The availabilty should not affect the color in cycle mode
    if (globalConfigMem[actionPtr + 2] != 3 && VMix[devIndex].getInputPropertiesType(globalConfigMem[actionPtr + 1] - 1) == 0) {
      retVal = 0;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7); // , pulses&B1 - not using this because it has no significance for this type of action.
      extRetValShortLabel(PSTR("Active"));
      extRetValLongLabel(PSTR("Active Input"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 3) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B110000 : B101010));
        extRetValSetLabel(true);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(globalConfigMem[actionPtr + 1] - 1), 0);
        extRetValTxtShort(VMix[devIndex].getInputPropertiesShortName(globalConfigMem[actionPtr + 1] - 1));
      } else {
        extRetValColor(!retVal ? 0 : B110101);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(VMix[devIndex].getActiveInput()), 0);
        extRetValTxtShort(VMix[devIndex].getInputPropertiesShortName(VMix[devIndex].getActiveInput()));
      }
    }
    return retVal;
    break;
  case 1: // Preview Source
    if (actDown) {
      if (globalConfigMem[actionPtr + 2] == 3) {       // Source cycle by button push
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used to show button is highlighted here
        pulses = 2;
      } else {
        VMix[devIndex].setPreviewInput(globalConfigMem[actionPtr + 1] - 1);
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 3) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      VMix[devIndex].setPreviewInput(pulsesHelper(VMix[devIndex].getPreviewInput(), globalConfigMem[actionPtr + 3] - 1, globalConfigMem[actionPtr + 1] - 1, true, pulses, 1, 1));
    }

    retVal = globalConfigMem[actionPtr + 2] == 3 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (3 | 0x20) : 5) : (VMix[devIndex].getPreviewInput() == (globalConfigMem[actionPtr + 1] - 1) ? (3 | 0x20) : 5);

    // The availabilty should not affect the color in cycle mode
    if (globalConfigMem[actionPtr + 2] != 3 && VMix[devIndex].getInputPropertiesType(globalConfigMem[actionPtr + 1] - 1) == 0) {
      retVal = 0;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7); // , pulses&B1 - not using this because it has no significance for this type of action.
      extRetValShortLabel(PSTR("Preview"));
      extRetValLongLabel(PSTR("Preview Input"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 3) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B110000 : B101010));
        extRetValSetLabel(true);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(globalConfigMem[actionPtr + 1] - 1), 0);
        extRetValTxtShort(VMix[devIndex].getInputPropertiesShortName(globalConfigMem[actionPtr + 1] - 1));
      } else {
        extRetValColor(!retVal ? 0 : B110101);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(VMix[devIndex].getPreviewInput()), 0);
        extRetValTxtShort(VMix[devIndex].getInputPropertiesShortName(VMix[devIndex].getPreviewInput()));
      }
    }
    return retVal;
    break;
  case 2: // Act/Preview Source
    if (actDown) {
      if (globalConfigMem[actionPtr + 2] == 3) {       // Source cycle by button push
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used to show button is highlighted here
        pulses = 2;
      } else {
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
        _systemHWcActionCache[HWc][actIdx] = millis();
        VMix[devIndex].setPreviewInput(globalConfigMem[actionPtr + 1] - 1);
      }
    }
    if (globalConfigMem[actionPtr + 2] != 3 && _systemHWcActionCacheFlag[HWc][actIdx]) {
      uint16_t t = millis();
      if (t - _systemHWcActionCache[HWc][actIdx] > 1000) {
        VMix[devIndex].performCutAction(true);
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
    }
    if (actUp) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      VMix[devIndex].setPreviewInput(pulsesHelper(VMix[devIndex].getPreviewInput(), globalConfigMem[actionPtr + 3] - 1, globalConfigMem[actionPtr + 1] - 1, true, pulses, 1, 1));
    }

    retVal = globalConfigMem[actionPtr + 2] == 3 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (3 | 0x20) : 5) : (VMix[devIndex].getActiveInput() == (globalConfigMem[actionPtr + 1] - 1) ? (2 | 0x20) : (VMix[devIndex].getPreviewInput() == (globalConfigMem[actionPtr + 1] - 1) ? (3 | 0x20) : 5));
    if (retVal != 5)
      retVal |= 0x20; // Add binary output bit
    if ((retVal & 0xF) == 3)
      retVal |= 0x10; // Bit 4 is a "mono-color blink flag" so a mono color button can indicate this special state.

    // The availabilty should not affect the color in cycle mode
    if (globalConfigMem[actionPtr + 2] != 3 && VMix[devIndex].getInputPropertiesType(globalConfigMem[actionPtr + 1] - 1) == 0) {
      retVal = 0;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Prg/Prv"));
      extRetValLongLabel(PSTR("Prg/Prv Src"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 3) {
        extRetValColor(!retVal ? 0 : ((retVal & 0xF) == 3 ? B001100 : ((retVal & 0xF) == 2 ? B110000 : B101010)));
        extRetValSetLabel(true);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(globalConfigMem[actionPtr + 1] - 1), 0);
        extRetValTxtShort(VMix[devIndex].getInputPropertiesShortName(globalConfigMem[actionPtr + 1] - 1));
      } else {
        extRetValColor(!retVal ? 0 : B101010);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(VMix[devIndex].getActiveInput()), 0);
        extRetVal2(0);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(VMix[devIndex].getPreviewInput()), 1);
      }
    }
    return retVal;
    break;
  case 3: // array(1+2+8, "OverlayInput", array(1,4,"Overlay"), array("Toggle","In","Out","Off","Zoom","Preview","Hold Down"), array(1,100,"Input"),array(1,100,"To")),	//
    if (actDown) {
      switch (globalConfigMem[actionPtr + 2]) {
      case 0: // Toggle
        if (VMix[devIndex].getOverlayActive(globalConfigMem[actionPtr + 1] - 1) && VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1) == (globalConfigMem[actionPtr + 3] - 1)) {
          VMix[devIndex].setOverlayInputOff(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 3]==101 ? VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1) : globalConfigMem[actionPtr + 3] - 1);
        } else {
          VMix[devIndex].setOverlayInputOn(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 3]==101 ? VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1) : globalConfigMem[actionPtr + 3] - 1);
        }
        break;
      case 1: // In
      case 6: // Hold down
        VMix[devIndex].setOverlayInputOn(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 3]==101 ? VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1) : globalConfigMem[actionPtr + 3] - 1);
        break;
      case 2: // Out
        VMix[devIndex].setOverlayInputOff(globalConfigMem[actionPtr + 1] - 1, true);
        break;
      case 3: // Off immediately
        VMix[devIndex].setOverlayInputInstantOff(globalConfigMem[actionPtr + 1] - 1, true);
        break;
      case 4: // Zoom
        VMix[devIndex].setOverlayInputZoom(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 3] - 1);
        break;
      case 5: // Preview
        VMix[devIndex].setOverlayInputPreview(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 3] - 1);
        break;
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 6) { // Hold down
      VMix[devIndex].setOverlayInputOff(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 3] - 1);
    }
    if (pulses & 0xFFFE) {
      if (globalConfigMem[actionPtr + 2] == 5) {
        VMix[devIndex].setOverlayInputPreview(globalConfigMem[actionPtr + 1] - 1, pulsesHelper(VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1), globalConfigMem[actionPtr + 3] - 1, globalConfigMem[actionPtr + 4] - 1, true, pulses, 1, 1));
      } else {
        if (VMix[devIndex].getOverlayActive(globalConfigMem[actionPtr + 1] - 1) && VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1) == (globalConfigMem[actionPtr + 3] - 1)) {
          VMix[devIndex].setOverlayInputOff(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 3] - 1);
        } else {
          VMix[devIndex].setOverlayInputOn(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 3] - 1);
        }
      }
    }
    retVal = (VMix[devIndex].getOverlayActive(globalConfigMem[actionPtr + 1] - 1) && VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1) == globalConfigMem[actionPtr + 3] - 1) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Overlay "), (globalConfigMem[actionPtr + 1] - 1) + 1);
      extRetValLongLabel(PSTR("Overlay "), (globalConfigMem[actionPtr + 1] - 1) + 1);
      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValColor(retVal & 0x20 ? B110111 : B101010);
        extRetValSetLabel(true);
        switch (globalConfigMem[actionPtr + 2]) {
        case 0: // Toggle
          extRetValTxt_P(PSTR("OnOff"), 0);
          break;
        case 1: // On
          extRetValTxt_P(PSTR("In"), 0);
          break;
        case 2: // Off
          extRetValTxt_P(PSTR("Out"), 0);
          break;
        case 3: // Off
          extRetValTxt_P(PSTR("Off"), 0);
          break;
        case 4: // Zoom
          extRetValTxt_P(PSTR("Zoom"), 0);
          break;
        case 5: // Preview
          extRetValTxt_P(PSTR("Preview"), 0);
          break;
        case 6: // Hold down
          extRetValTxt_P(PSTR("Hold"), 0);
          extRetValTxt_P(PSTR("Down"), 1);
          extRetVal2(0);
          break;
        }
      } else {
        if (globalConfigMem[actionPtr + 2] != 5) {
          extRetValColor(B101010);
          extRetValTxt_P(retVal & 0x20 ? PSTR("On") : PSTR("Off"), 0);
        } else {
          extRetValTxt(VMix[devIndex].getInputPropertiesLongName(VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1)), 0);
          extRetValTxtShort(VMix[devIndex].getInputPropertiesShortName(VMix[devIndex].getOverlayInput(globalConfigMem[actionPtr + 1] - 1)));
          extRetVal2(0);
        }
      }
    }
    return retVal;
    break;
  case 4: // CUT
    if (actDown || (pulses & 0xFFFE)) {
      VMix[devIndex].performCutAction(true);
      _systemHWcActionCacheFlag[HWc][actIdx] = true;
    }
    if (actUp) {
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    retVal = _systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValColor(retVal & 0x20 ? B111111 : B101010);
      if (_systemHWcActionPrefersLabel[HWc])
        extRetValSetLabel(true);
      extRetValTxt_P(PSTR("CUT"), 0);
    }

    return retVal;
    break;
  case 5: // Fade
    if (actDown || (pulses & 0xFFFE)) {
      VMix[devIndex].performFadeFader(globalConfigMem[actionPtr + 1] - 1);
      _systemHWcActionCacheFlag[HWc][actIdx] = true;
    }
    if (actUp) {
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    retVal = _systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValColor(retVal & 0x20 ? B110101 : B101010);
      if (_systemHWcActionPrefersLabel[HWc])
        extRetValSetLabel(true);
      extRetValTxt_P(PSTR("FADE"), 0);
    }
    return retVal;
    break;
  case 6: // FTB
    if (actDown || (pulses & 0xFFFE)) {
      VMix[devIndex].performFadeToBlackAction(!VMix[devIndex].getFadeToBlackActive());
    }

    retVal = VMix[devIndex].getFadeToBlackActive() ? ((millis() & 512) > 0 ? 2 : 0) : 5;
    if (retVal != 5)
      retVal |= 0x20;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValColor(retVal & 0x20 ? B110101 : B101010);
      if (_systemHWcActionPrefersLabel[HWc])
        extRetValSetLabel(true);
      extRetValTxt_P(PSTR("FTB"), 0);
    }
    return retVal;
    break;
  case 7:                          // Transition Pos
    if (actDown) {                 // Use actDown as "has moved"
      if (value != BINARY_EVENT) { // Value input
        if (actUp) {               // Use actUp as "at end"
          VMix[devIndex].setTransitionPosition(value > 500 ? 255 : 0);
        } else {
          VMix[devIndex].setTransitionPosition(map(value, 0, 1000, 0, 255));
        }
      } else { // Binary - reset
        VMix[devIndex].setTransitionPosition(0);
      }
    }
    break;
  case 9: // Change Value
    if (actDown) {
      VMix[devIndex].setXamlValue(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 2], VMix[devIndex].getXamlValue(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 2]) + globalConfigMem[actionPtr + 4] * (globalConfigMem[actionPtr + 3] == 1 ? -1 : 1));
    }
    break;
  case 10: // Count Down
    if (actDown) {
      uint8_t cDmode = 0;
      if (globalConfigMem[actionPtr + 3] == 0) {
        _systemHWcActionCache[HWc][actIdx] = (_systemHWcActionCache[HWc][actIdx] + 1) % 2;
        cDmode = _systemHWcActionCache[HWc][actIdx] + 1;
      } else {
        cDmode = globalConfigMem[actionPtr + 3] - 1;
      }
      VMix[devIndex].setCountDownMode(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 2], cDmode);
    }
    retVal = _systemHWcActionCache[HWc][actIdx] ? (4 | 0x20) : 5;
    return retVal;
    break;
  case 11: // Stream
    if (actDown) {
      VMix[devIndex].performStreamAction(!VMix[devIndex].getStreamActive());
    }
    retVal = VMix[devIndex].getStreamActive() ? (2 | 0x20) : 5;
    return retVal;
    break;
  case 12: // Record
    if (actDown) {
      VMix[devIndex].performRecordAction(!VMix[devIndex].getRecordActive());
    }
    retVal = VMix[devIndex].getRecordActive() ? (2 | 0x20) : 5;
    return retVal;
    break;
  case 13: // Replay setup
    if (actDown) {
      VMix[devIndex].setReplayPropertiesSeconds(globalConfigMem[actionPtr + 1]);
      VMix[devIndex].setReplayPropertiesSpeed(globalConfigMem[actionPtr + 2] == 0 ? 25 : 50);
    }
    break;
  case 14: // Replay
    if (actDown) {
      VMix[devIndex].performReplayAction(true);
    }
    break;
  case 15: // Transition To
    if (actDown) {
      VMix[devIndex].setActiveInputTransitionDuration(globalConfigMem[actionPtr + 1] - 1, globalConfigMem[actionPtr + 2], globalConfigMem[actionPtr + 3] * 100);
    }

    retVal = VMix[devIndex].getActiveInput() == (globalConfigMem[actionPtr + 1] - 1) ? (2 | 0x20) : 5;

    // The availabilty should not affect the color in cycle mode
    if (VMix[devIndex].getInputPropertiesType(globalConfigMem[actionPtr + 1] - 1) == 0) {
      retVal = 0;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7); // , pulses&B1 - not using this because it has no significance for this type of action.
      extRetValShortLabel(PSTR("Trans. To"));
      extRetValLongLabel(PSTR("Transition To"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 3) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B110000 : B101010));
        extRetValSetLabel(true);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(globalConfigMem[actionPtr + 1] - 1), 0);
        extRetValTxtShort(VMix[devIndex].getInputPropertiesShortName(globalConfigMem[actionPtr + 1] - 1));
      } else {
        extRetValColor(!retVal ? 0 : B110101);
        extRetValTxt(VMix[devIndex].getInputPropertiesLongName(VMix[devIndex].getActiveInput()), 0);
        extRetValTxtShort(VMix[devIndex].getInputPropertiesShortName(VMix[devIndex].getActiveInput()));
      }
    }
    return retVal;
    break;
  }

  // Default:
  if (actDown) {
    _systemHWcActionCacheFlag[HWc][actIdx] = true;
  }
  if (actUp) {
    _systemHWcActionCacheFlag[HWc][actIdx] = false;
  }
  return _systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5;
}
}