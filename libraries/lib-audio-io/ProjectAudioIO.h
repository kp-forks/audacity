/**********************************************************************

Audacity: A Digital Audio Editor

ProjectAudioIO.h

Paul Licameli split from AudacityProject.h

**********************************************************************/

#ifndef __PROJECT_AUDIO_IO__
#define __PROJECT_AUDIO_IO__

#include "ClientData.h" // to inherit
#include "GlobalVariable.h"
#include "Observer.h" // to inherit
#include <wx/weakref.h>

#include <atomic>
#include <memory>
class AudacityProject;
struct AudioIOStartStreamOptions;
class Meter;

struct SpeedChangeMessage {};

///\ brief Holds per-project state needed for interaction with AudioIO,
/// including the audio stream token and pointers to meters
class AUDIO_IO_API ProjectAudioIO final
   : public ClientData::Base
   , public Observer::Publisher<SpeedChangeMessage>
{
public:
   //! Type of function constructing AudioIOStartStreamOptions
   using OptionsFactory =
      AudioIOStartStreamOptions(
         AudacityProject &project, bool newDefaults);

   //! Function returning a default factory function, which ignores the
   //! second argument
   static const std::function<OptionsFactory> DefaultOptionsFactory();

   //! Global hook making AudioIOStartStreamOptions for a project, which
   //! has a non-trivial default implementation
   struct AUDIO_IO_API DefaultOptions : GlobalHook< DefaultOptions,
      OptionsFactory,
      DefaultOptionsFactory // default installed implementation
   >{};

   //! Invoke the global hook, supplying a default argument
   static AudioIOStartStreamOptions GetDefaultOptions(
      AudacityProject &project,
      bool newDefaults = false /*!< if true, policy is meant to respond to
         looping region; but specifying that is outside this library's scope
      */
   );

   static ProjectAudioIO &Get( AudacityProject &project );
   static const ProjectAudioIO &Get( const AudacityProject &project );

   explicit ProjectAudioIO( AudacityProject &project );
   ProjectAudioIO( const ProjectAudioIO & ) = delete;
   ProjectAudioIO &operator=( const ProjectAudioIO & ) = delete;
   ~ProjectAudioIO();

   int GetAudioIOToken() const;
   bool IsAudioActive() const;
   void SetAudioIOToken(int token);

   const std::shared_ptr<Meter> &GetPlaybackMeter() const;
   void SetPlaybackMeter(
      const std::shared_ptr<Meter> &playback);
   const std::shared_ptr<Meter> &GetCaptureMeter() const;
   void SetCaptureMeter(
      const std::shared_ptr<Meter> &capture);

   // Speed play
   double GetPlaySpeed() const {
      return mPlaySpeed.load( std::memory_order_relaxed ); }
   void SetPlaySpeed( double value );

private:
   AudacityProject &mProject;

   // Project owned meters
   std::shared_ptr<Meter> mPlaybackMeter;
   std::shared_ptr<Meter> mCaptureMeter;

   // This is atomic because scrubber may read it in a separate thread from
   // the main
   std::atomic<double> mPlaySpeed{};

   int  mAudioIOToken{ -1 };
};

#endif
