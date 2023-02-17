/*!********************************************************************

  Audacity: A Digital Audio Editor

  @file AudioUnitValidator.h

  Dominic Mazzoni
  Leland Lucius

  Paul Licameli split from AudioUnitEffect.cpp

**********************************************************************/

#ifndef __AUDACITY_AUDIO_UNIT_VALIDATOR__
#define __AUDACITY_AUDIO_UNIT_VALIDATOR__

#include <AudioToolbox/AudioUnitUtilities.h>
#include <unordered_map>
#include "AudioUnitUtils.h"
#include "EffectPlugin.h"

class AUControl;

class AudioUnitInstance;

class AudioUnitValidator : public wxEvtHandler, public EffectUIValidator {
   struct CreateToken{};
public:
   static std::unique_ptr<EffectUIValidator> Create(
      EffectUIClientInterface &effect, ShuttleGui &S,
      const wxString &uiType,
      EffectInstance &instance, EffectSettingsAccess &access);

   AudioUnitValidator(CreateToken,
      EffectUIClientInterface &effect, EffectSettingsAccess &access,
      AudioUnitInstance &instance, AUControl *pControl, bool isGraphical);

   ~AudioUnitValidator() override;

   bool UpdateUI() override;
   bool ValidateUI() override;
   bool IsGraphicalUI() override;

private:
   static void EventListenerCallback(void *inCallbackRefCon,
      void *inObject, const AudioUnitEvent *inEvent,
      UInt64 inEventHostTime, AudioUnitParameterValue inParameterValue);
   void EventListener(const AudioUnitEvent *inEvent,
      AudioUnitParameterValue inParameterValue);
   void OnIdle(wxIdleEvent &evt);
   bool FetchSettingsFromInstance(EffectSettings &settings);
   bool StoreSettingsToInstance(const EffectSettings &settings);

   void Notify();

   using EventListenerPtr =
      AudioUnitCleanup<AUEventListenerRef, AUListenerDispose>;

   EventListenerPtr MakeListener();

   // The lifetime guarantee is assumed to be provided by the instance.
   // See contract of PopulateUI
   AudioUnitInstance &mInstance;
   const EventListenerPtr mEventListenerRef;
   AUControl *const mpControl{};
   std::vector<
      std::pair<AudioUnitParameterID, AudioUnitParameterValue>> mToUpdate;
   const bool mIsGraphical;

   // The map of parameter IDs to their current values
   std::unordered_map<AudioUnitParameterID, AudioUnitParameterValue>
      mParameterValues;
};
#endif
