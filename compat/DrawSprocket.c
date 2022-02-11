#include <Carbon/Carbon.h>
#include "MacTypes.h"
#include "DrawSprocket.h"

/**
 * Initializes DrawSprocket.
 * @return A result code.
 *
 * You must call this function before attempting to call any DrawSprocket
 * functions (except for DSpGetVersion (page 45)).
 */
OSStatus DSpStartup ()
{
    return noErr;
}

/**
 * Shuts down DrawSprocket.
 * @return A result code.
 *
 * You must call this function before quitting the application.
 */
OSStatus DSpShutdown ()
{
    return noErr;
}

/**
 * Completely fades in a display to a color of your choice.
 *
 * @param [in] inContext
 *   A reference to the context whose display is to be faded. If you pass
 *   <code>NULL</code> for this parameter, the fade operation applies
 *   simultaneously to all displays.
 *
 * @param [in] inZeroIntensityColor
 *   The color that is to correspond to zero intensity. If you pass
 *   <code>NULL</code> for this parameter, the zero-intensity color is black.
 *
 * @return A result code.
 *
 * <p>
 * The DSpContext_FadeGammaIn function automatically performs a gamma fade on
 * the display of the context specified in the inContext parameter. It fades the
 * display from 0 percent to 100 percent intensity over a period of one second.
 * You specify the zero-intensity color in the inZeroIntensityColor parameter.
 * A key press or a mouse-button click will jump the fade to its end point
 * immediately.
 * </p>
 * <p>
 * You can perform a manual fade with the DSpContext_FadeGamma function (page
 * 2-45). For a complete discussion of gamma fades see "Gamma Fading" (page
 * 2-9).
 * </p>
 */
OSStatus DSpContext_FadeGammaIn (
    DSpContextReference inContext,
    RGBColor *inZeroIntensityColor)
{
    return noErr;
}

/**
 * Completely fades out a display to a color of your choice.
 *
 * @param [in] inContext
 *   A reference to the context whose display is to be faded. If you pass
 *   <code>NULL</code> for this parameter, the fade operation applies
 *   simultaneously to all displays.
 *
 * @param [in] inZeroIntensityColor
 *   A pointer to the color that is to correspond to zero intensity. If you pass
 *   <code>NULL</code> for this parameter, the zero-intensity color is black.
 *
 * @return A result code.
 *
 * <p>
 * The DSpContext_FadeGammaOut function automatically performs a gamma fade on
 * the display of the context specified in the inContext parameter. It fades the
 * display from 100 percent to 0 percent intensity over a period of one second.
 * You specify the zero-intensity color in the inZeroIntensityColor parameter.
 * A key press or a mouse button click will jump the fade to its end point
 * immediately.
 * </p>
 * <p>
 * You can perform a manual fade with the DSpContext_FadeGamma function
 * (page 2-45). For a complete discussion of gamma fades see "Gamma Fading"
 * (page 2-9).
 * </p>
 */
OSStatus DSpContext_FadeGammaOut (
    DSpContextReference inContext,
    RGBColor *inZeroIntensityColor)
{
    return noErr;
}

/**
 * Obtains the front buffer for the context.
 *
 * @param [in] inContext
 *   A reference to the context whose front buffer is to be returned.
 *
 * @param [out] outFrontBuffer
 *   On return, a pointer to the front buffer (that is, to a <code>CGrafPort</code>).
 *
 * @return A result code.
 *
 * <p>
 * The front buffer is the screen display. Typically you use this function when
 * you are not using backbuffers and you want to pass a CGrafPtr so another
 * interface can draw to the screen (for example, by using OpenGL or QuickTime,
 * or you simply want to change resolutions). However, if you are drawing to the
 * screen yourself, you must call DSpContext_GetFrontBuffer each time through
 * your game’s drawing cycle to compensate for possible page flipping.
 * </p>
 * <p>
 * Note that 3D hardware accelerators (such as RAVE) typically must draw using a
 * graphics device (GDevice) rather than a graphics port. To do so, you should
 * call DSpContext_GetDisplayID (page 29) to get the display ID of the device
 * the context is on and then call the Display Manager function
 * DMGetDeviceByDisplayID to obtain the GDevice.
 * </p>
 */
OSStatus DSpContext_GetFrontBuffer (
    DSpContextReferenceConst inContext,
    CGrafPtr *outFrontBuffer)
{
    return noErr;
}

/**
 * Releases a context you are finished using.
 *
 * @param [in] inContext
 *   A reference to the context to be released.
 *
 * @return A result code.
 *
 * <p>
 * When you are finished using a context, you must release it with the
 * DSpContext_Release function. You pass the function a reference to the context
 * to be released in the inContext parameter. Releasing the context does not
 * necessarily remove the blanking window from its graphics device's display.
 * All displays remain covered by the blanking window until all contexts have
 * been released or put in an inactive play state.
 * </p>
 */
OSStatus DSpContext_Release (
        DSpContextReference inContext)
{
    return noErr;
}

/**
 * Reserves a context so that you can begin using it in your game.
 *
 * @param [in] inContext
 *   A reference to the context to reserve.
 *
 * @param [in] inDesiredAttributes
 *   A pointer to an attributes structure that specifies the configuration you
 *   would like for the display when it is in the active or paused state.
 *
 * @return A result code.
 *
 * <p>
 * You pass the function a reference to the context to reserve in the
 * inContext parameter. When the context is reserved, it is in the inactive
 * state. There will be no visible indication that the context has been reserved
 * at this point. To enable your context, call DSpContext_SetState (page 2-43).
 * The context will show up on the display once the context has been placed in
 * the active state.
 * </p>
 * <p>
 * The attributes structure you can specify in the DSpContextAttributesPtr
 * parameter provides the configuration information for the display when it
 * is in the active or paused states. If you would like to override the
 * attributes of the context, you may do so in the attributes structure. For
 * example, if you ask for a 320x240x16 display but the closest match is a
 * context that is 640x480x32, passing in your requested attributes when you
 * reserve the context will cause the DSpContext_GetBackBuffer function to
 * return a graphics pointer that refers to a 320x240x16 drawing environment.
 * </p>
 * <p>
 * Along the same lines, you should turn off features that you are not
 * interested in when you reserve the context. For example, if the context
 * supports page flipping (and you know this because you requested the
 * actual capabilities of the context using DSpContext_GetAttributes), you
 * can turn off the page-flipping bit in your desired attributes so that you
 * will be assured of using software buffering.
 * </p>
 * <p>
 * You should only specify a back buffer bit depth different from the
 * display bit depth when you absolutely must, as it is the worst case
 * scenario for DrawSprocket and will result in a synchronous call to
 * CopyBits to bring your back buffer to the display.
 * </p>
 */
OSStatus DSpContext_Reserve (
        DSpContextReference inContext,
        DSpContextAttributesPtr inDesiredAttributes)
{
    return noErr;
}

/**
 * Sets the play state of a context.
 *
 * @param inContext
 *   A reference to the context whose play state you want to set.
 *
 * @param inState
 *   The state to set the context to. Valid input values for this parameter
 *   are kDSpContextState_Active, kDSpContextState_Paused, and
 *   kDSpContextState_Inactive.
 *
 * @return A result code.
 *
 * <p>
 * The DSpContext_SetState function sets the play state of the context
 * specified in the inContext parameter to the state specified in the
 * inState parameter. See "Play State" (page 2-25) for more information on
 * valid values for the inPlayState parameter. In summary, you can make
 * these choices:
 *
 * <ul>
 * <li>
 * A context's initial play state is inactive. When all contexts for a
 * display are set to kDSpContextState_Inactive, the display looks exactly
 * as it does when the user is using their Macintosh normally: the monitor
 * resolutions are set to the default, the menu bar is available, and so on.
 * </li>
 * <li>
 * Set the play state to kDSpContextState_Active to use the display. In this
 * state, the attributes of the context are used to change the display
 * resolution, remove the menu bar, and so on. When at least one context is
 * active, all the display devices in the system are covered by a blanking
 * window. When a context is in the active state, the display is completely
 * owned by the game.
 * </li>
 * <li>
 * Set the play state to kDSpContextState_Paused to temporarily restore
 * system adornments, while maintaining the attributes used by the context.
 * This gives the user the opportunity to use the menus and switch to other
 * applications. While the context is in the paused state, it is very
 * important to call DSpProcessEvent to allow DrawSprocket to correctly
 * handle events such as suspend or resume (see the DSpProcessEvent function
 * on (page 2-72). Page flipping and double buffering are inactive in this
 * state, and the context will be placed back at page 0 if page flipping was
 * being used.
 * </li>
 * </ul>
 *
 * </p>
 */
OSStatus DSpContext_SetState (
    DSpContextReference inContext,
    DSpContextState inState)
{
    return noErr;
}

/**
 * Finds the context that best matches a context you describe.
 *
 * @param [in] inDesiredAttributes
 *   A pointer to a context attributes structure. You should specify as best as
 *   possible the details of a context you would like to use.
 *
 * @param [out] outContext
 *   On exit, a reference to the context that best meets or exceeds the
 *   specified attribute requirements.
 *
 * @return A result code.
 *
 * <p>
 * The function DSpFindBestContext takes, in the inDesiredAttributes
 * parameter, a pointer to a context attributes structure describing display
 * characteristics, such as display height and width, preferred pixel depth,
 * and color capability, that your program needs. It returns, in the
 * outContext parameter, a reference to the context that best matches the
 * description. All display contexts are considered that meet or exceed the
 * requirements in the attributes. If no such contexts exist, then an error
 * is returned.
 * </p>
 * <p>
 * The game should check the attributes of the chosen context by calling
 * DSpContext_GetAttributes. It is possible that the game may want to use
 * attributes of the context that exceed those asked for. For example, the
 * game may request a mode such as 320x200x8 but the best match is a
 * 640x480x8 display; the game can adapt to a full screen mode once it is
 * aware of the situation.
 * </p>
 */
OSStatus DSpFindBestContext (
    DSpContextAttributesPtr inDesiredAttributes,
    DSpContextReference *outContext)
{
    return noErr;
}
