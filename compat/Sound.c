#include <Carbon/Carbon.h>

/**
 * SysBeep
You can use the SysBeep procedure to play the system alert sound.

PROCEDURE SysBeep (duration: Integer);

 @param duration
    The duration (in ticks) of the resulting sound. This parameter is ignored except on a Macintosh Plus, Macintosh SE, or Macintosh Classic when the system alert sound is the Simple Beep. The recommended duration is 30 ticks, which equals one-half second.

 @remarks
DESCRIPTION

The SysBeep procedure causes the Sound Manager to play the system alert sound at its current volume. If necessary, the Sound Manager loads into memory the sound resource containing the system alert sound and links it to a sound channel. The user selects a system alert sound in the Alert Sounds subpanel of the Sound control panel.

The volume of the sound produced depends on the current setting of the system alert sound volume, which the user can adjust in the Alert Sounds subpanel of the Sound control panel. The system alert sound volume can also be read and set by calling the GetSysBeepVolume and SetSysBeepVolume routines. If the volume is set to 0 (silent) and the system alert sound is enabled, calling SysBeep causes the menu bar to blink once.

SPECIAL CONSIDERATIONS
Because the SysBeep procedure moves memory, you should not call it at interrupt time.

SEE ALSO
For information on enabling and disabling the system alert sound, see the description of SndGetSysBeepState and SndGetSysBeepState on page 2-137. For information on reading or adjusting the system alert sound volume, see "Controlling Volume Levels" beginning on page 2-139.
 */
void SysBeep(short duration)
{
  // NOT IMPLEMENTED
}

/**
<h3>SndDoImmediate</h3>
 You can use the <code>SndDoImmediate</code> function to place a sound command in front of a sound channel's command queue.
 <p></p>
 <pre>FUNCTION SndDoImmediate (chan: SndChannelPtr; cmd: SndCommand):
                           OSErr;
</pre>
<dl>
<dt><code>chan</code>
</dt><dd> A pointer to a sound channel.
</dd><dt><code>cmd</code>
</dt><dd> A sound command to be sent to the channel specified in the <code>chan</code> parameter.
</dd></dl>
<a name="HEADING101-5"></a>
<h5>DESCRIPTION</h5>
 The <code>SndDoImmediate</code> function operates much like <code>SndDoCommand</code>, except that it bypasses the existing command queue of the sound channel and sends the specified command directly to the Sound Manager for immediate processing. This routine also overrides any <code>waitCmd</code>, <code>pauseCmd</code>, or <code>syncCmd</code> commands that might have already been processed. However, other commands already received by the Sound Manager will not be interrupted by the <code>SndDoImmediate</code> function (although a <code>quietCmd</code> command sent via <code>SndDoImmediate</code> will quiet a sound already playing).<p>
<a name="HEADING101-7"></a>
</p><h5>SPECIAL CONSIDERATIONS</h5>
 Whether <code>SndDoImmediate</code> moves memory depends on the particular sound command you're sending it. Most of the available sound commands do not cause <code>SndDoImmediate</code> to move memory and can therefore be issued at interrupt time. Moreover, you can sometimes safely send commands at interrupt time that would otherwise cause memory to move if you've previously issued the <code>soundCmd</code> sound command to preconfigure the channel at noninterrupt time.<a name="MARKER-2-862"></a><a name="MARKER-2-863"></a><p>
<a name="HEADING101-9"></a>
</p><h5>RESULT CODES
<table><tbody><tr><td>noErr</td><td>0</td><td>No error</td></tr><tr>
<td>badChannel</td><td>-205</td><td>Channel is corrupt or unusable</td></tr></tbody></table>
</h5>
<h5>SEE ALSO</h5>
 For an example of a routine that uses the <code>SndDoImmediate</code> function, see Listing 2-4 on page 2-26.
 */
OSErr
SndDoImmediate(
    SndChannelPtr       chan,
    const SndCommand *  cmd)
{
  // NOT IMPLEMENTED
  return noErr;
}

/**
<h3>SndDoCommand</h3>

 You can queue a command in a sound channel by calling the <code>SndDoCommand</code> function.<p>
</p>

<pre>FUNCTION SndDoCommand (chan: SndChannelPtr; cmd: SndCommand;
                        noWait: Boolean): OSErr;
</pre>

 <dl>
<dt><code>chan</code>
</dt><dd> A pointer to a valid sound channel.
</dd><dt><code>cmd</code>
</dt><dd> A sound command to be sent to the channel specified in the <code>chan</code> parameter.
</dd><dt><code>noWait</code>
</dt><dd> A flag indicating whether the Sound Manager should wait for a free space in a full queue (<code>FALSE</code>) or whether it should return immediately with a <code>queueFull</code> result code if the queue is full (<code>TRUE</code>).
</dd></dl>

<h5>DESCRIPTION</h5>
 The <code>SndDoCommand</code> function sends the sound command specified in the <code>cmd</code> parameter to the end of the command queue of the channel specified in the <code>chan</code> parameter.<p>
 The <code>noWait</code> parameter has meaning only if a sound channel's queue of sound commands is full. If the <code>noWait</code> parameter is set to <code>FALSE</code> and the queue is full, the Sound Manager waits until there is space to add the command, thus preventing your application from doing other processing. If <code>noWait</code> is set to <code>TRUE</code> and the queue is full, the Sound Manager does not send the command and returns the <code>queueFull</code> result code.</p><p>
</p>

 <h5>SPECIAL CONSIDERATIONS</h5>
 Whether <code>SndDoCommand</code> moves memory depends on the particular sound command you're sending it. Most of the available sound commands do not cause <code>SndDoCommand</code> to move memory and can therefore be issued at interrupt time. Moreover, you can sometimes safely send commands at interrupt time that would otherwise cause memory to move if you've previously issued the <code>soundCmd</code> sound command to preconfigure the channel at noninterrupt time.<p>
</p>

 <h5>RESULT CODES
<table><tbody><tr><td>noErr</td><td>0</td><td>No error</td></tr><tr>
<td>queueFull</td><td>-203</td><td>No room in the queue</td></tr><tr>
<td>badChannel</td><td>-205</td><td>Channel is corrupt or unusable</td></tr></tbody></table>
</h5>

<h5>SEE ALSO</h5>
 For an example of a routine that uses the <code>SndDoCommand</code> function, see Listing 2-15 on page 2-42.
 */
OSErr
SndDoCommand(
    SndChannelPtr       chan,
    const SndCommand *  cmd,
    Boolean             noWait)
{
  // NOT IMPLEMENTED
  return noErr;
}

static OSErr SndChannel_Alloc(SndChannelPtr *result)
{
  SndChannel *self = (SndChannel *) NewPtr(sizeof(SndChannel));
  OSErr err = MemError();
  if (err != noErr) { return err; }

  *result = self;
  return noErr;
}

static OSErr SndChannel_Dispose(SndChannelPtr chan)
{
  if (chan == NULL) { return noErr; }
  DisposePtr((Ptr) chan);
  return noErr;
}

/**
*
<h3>SndNewChannel<</h3>
 You can use the <code>SndNewChannel</code> function to allocate a new sound channel.<p>
</p><pre>FUNCTION SndNewChannel (VAR chan: SndChannelPtr; synth: Integer;
                        init: LongInt; userRoutine: ProcPtr):
                        OSErr;
</pre>
<dl>
<dt><code>chan</code>
</dt><dd> A pointer to a sound channel record. You can pass a pointer whose value is <code>NIL</code> to force the Sound Manager to allocate the sound channel record internally.
</dd><dt><code>synth</code>
</dt><dd> The sound data type you intend to play on this channel. If you do not want to specify a specific data type, pass 0 in this parameter. You might do this if you plan to use the channel to play a single sound resource that itself specifies the sound's data type.
</dd><dt><code>init</code>
</dt><dd> The desired initialization parameters for the channel. If you cannot determine what types of sounds you will be playing on the channel, pass 0 in this parameter. Only sounds defined by wave-table data and sampled-sound data currently use the <code>init</code> options. You can use the <code>Gestalt</code> function to determine if a sound feature (such as stereo output) is supported by a particular computer.
</dd><dt><code>userRoutine</code>
</dt><dd> A pointer to a callback procedure that the Sound Manager executes whenever it receives a <code>callBackCmd</code> command. If you pass <code>NIL</code> as the <code>userRoutine</code> parameter, then any <code>callBackCmd</code> commands sent to this channel are ignored.
</dd></dl>
<h5>DESCRIPTION</h5>
 The <code>SndNewChannel</code> function internally allocates memory to store a queue of sound commands. If you pass a pointer to <code>NIL</code> as the <code>chan</code> parameter, the function also allocates a sound channel record in your application's heap and returns a pointer to that record. If you do not pass a pointer to <code>NIL</code> as the <code>chan</code> parameter, then that parameter must contain a pointer to a sound channel record.<p>
 If you pass a pointer to <code>NIL</code> as the <code>chan</code> parameter, then the amount of memory the <code>SndNewChannel</code> function allocates to store the sound commands is enough to store 128 sound commands. However, if you pass a pointer to the sound channel record rather than a pointer to <code>NIL</code>, the amount of memory allocated is determined by the <code>qLength</code> field of the sound channel record. Thus, if you wish to control the size of the sound queue, you must allocate your own sound channel record. Regardless of whether you allocate your own sound channel record, the Sound Manager allocates memory for the sound command queue internally.</p><p>
 The <code>synth</code> parameter specifies the sound data type you intend to play on this channel. You can use these constants to specify the data type:</p><p>
</p><pre>CONST
   squareWaveSynth         = 1;        {square-wave data}
   waveTableSynth          = 3;        {wave-table data}
   sampledSynth            = 5;        {sampled-sound data}
</pre>
 In Sound Manager versions earlier than version 3.0, only one data type can be produced at any one time. As a result, <code>SndNewChannel</code> may fail if you attempt to open a channel specifying a data type other than the one currently being played.<p>
 To specify a sound output device other than the current sound output device, pass the value <code>kUseOptionalOutputDevice</code> in the <code>synth</code> parameter and the signature of the desired sound output device component in the <code>init</code> parameter.</p><p>
</p><pre>CONST
   kUseOptionalOutputDevice      = -1;
</pre>
 The ability to redirect output away from the current sound output device is intended for use by specialized applications that need to use a specific sound output device. In general, your application should always send sound to the current sound output device selected by the user.<p>
<a name="HEADING97-16"></a>
</p><h5>SPECIAL CONSIDERATIONS</h5>
 Because the <code>SndNewChannel</code> function allocates memory, you should not call it at interrupt time.<p>
<a name="HEADING97-18"></a>
</p><h5>RESULT CODES
<table><tbody><tr><td>noErr</td><td>0</td><td>No error</td></tr><tr>
<td>resProblem</td><td>-204</td><td>Problem loading the resource</td></tr><tr>
<td>badChannel</td><td>-205</td><td>Channel is corrupt or unusable</td></tr></tbody></table>
</h5>
<a name="HEADING97-19"></a>
<h5>SEE ALSO</h5>
 For an example of a routine that uses the <code>SndNewChannel</code> function, see Listing 2-1 on page 2-20</a>.
 For information on the format of a callback procedure, see "Callback Procedures" on page 2-152</a>.
</p>
*/
OSErr
SndNewChannel(
    SndChannelPtr *  chan,
    short            synth,
    long             init,
    SndCallBackUPP   userRoutine)
{
  OSErr err;
  if (*chan == NULL) {
    err = SndChannel_Alloc(chan);
    if (err != noErr) { return err; }
  }

  return noErr;
}

/**
*
<h3>SndDisposeChannel</h3>
 If you allocate a sound channel by calling the <code>SndNewChannel</code> function, you must release the memory it occupies by calling the <code>SndDisposeChannel</code> function. <p>
</p><pre>FUNCTION SndDisposeChannel (chan: SndChannelPtr;
                              quietNow: Boolean): OSErr;
</pre>
<dl>
<dt><code>chan</code>
</dt><dd> A pointer to a valid sound channel record.
</dd><dt><code>quietNow</code>
</dt><dd> A Boolean value that indicates whether the channel should be disposed immediately (<code>TRUE</code>) or after sound stops playing (<code>FALSE</code>).
</dd></dl>
<a name="HEADING98-5"></a>
<h5>DESCRIPTION</h5>
 The <code>SndDisposeChannel</code> function disposes of the queue of sound commands associated with the sound channel specified in the <code>chan</code> parameter. If your application created its own sound channel record in memory or installed a sound as a voice in a channel, the Sound Manager does not dispose of that memory. The Sound Manager also does not release memory associated with a sound resource that you have played on a channel. You might use the <code>userInfo</code> field of the sound channel record to store the address of a sound handle you wish to release before disposing of the sound channel itself.<p>
 The <code>SndDisposeChannel</code> function can dispose of a channel immediately or wait until the queued commands are processed. If <code>quietNow</code> is set to <code>TRUE</code>, a <code>flushCmd</code> command and then a <code>quietCmd</code> command are sent to the channel bypassing the command queue. This removes all commands, stops any sound in progress, and closes the channel. If <code>quietNow</code> is set to <code>FALSE</code>, then the Sound Manager issues a <code>quietCmd</code> command only; it does not bypass the command queue, and it waits until the <code>quietCmd</code> command is processed before disposing of the channel.<a name="MARKER-9-547"></a><a name="MARKER-2-852"></a></p><p>
</p><h5>SPECIAL CONSIDERATIONS</h5>
 Because the <code>SndDisposeChannel</code> function might dispose of memory, you should not call it at interrupt time.<p>
</p><h5>RESULT CODES</h5>
<table><tbody><tr><td>noErr</td><td>0</td><td>No error</td></tr><tr>
<td>badChannel</td><td>-205</td><td>Channel is corrupt or unusable</td></tr></tbody></table>
*/
OSErr
SndDisposeChannel(
    SndChannelPtr   chan,
    Boolean         quietNow)
{
  // NOT FULLY IMPLEMENTED
  return SndChannel_Dispose(chan);
}