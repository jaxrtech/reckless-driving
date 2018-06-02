#include "error.h"
#include "InternetConfig.h"
#include "screen.h"
#include "interface.h"
#include "preferences.h"
#include "packs.h"
#include "initexit.h"
#include "ZonicKRM.h"
#include <math.h>
#define kCreator 'Råç2'

UInt32 gKey;
int gRegistered=false;

enum{
	kRegAppButton=1,
	kOnlineButton,
	kCodeButton,
	kCancelButton
};

enum{
	kCodeOKButton=1,
	kCodeCancelButton,
	kCodeNameField,
	kCodeCodeField
};

char Hex2Num(char hex)
{
	if(hex<='9')return hex-'0';
	else return hex-'A'+10;
}

UInt32 CodeStr2Long(Str255 code)
{
	UInt32 codeNum=0;
	UInt32 seed=0;
	int digi;
	if(code[0]!=10) return 0;
	for(digi=10;digi>0;digi--)
		if(!((code[digi]>='0'&&code[digi]<='9')||(code[digi]>='A'&&code[digi]<='F')))
			return 0;
	for(digi=10;digi>8;digi--)
		seed+=Hex2Num(code[digi])*pow(16,10-digi);
	seed=seed+(seed<<8)+(seed<<16)+(seed<<24);
	for(digi=8;digi>0;digi--)
		codeNum+=Hex2Num(code[digi])*pow(16,8-digi);
	codeNum^=seed;
	return codeNum;
}

void StripSpaces(Str255 str)
{
	int i;
	for(i=1;i<=str[0];i++)
		if(str[i]==' ')
		{
			BlockMoveData(str+i+1,str+i,str[0]-i);
			str[0]--;
			i--;
		}
}


int CheckRegi()
{
	UInt32 *nameNum,codeNum;
	Str255 upName;
	Handle check;
	
	BlockMoveData(gPrefs.name,upName,gPrefs.name[0]+1);
	UpperString(upName,false);
	StripSpaces(upName);
	nameNum=upName+upName[0]-3;
	codeNum=CodeStr2Long(gPrefs.code);
	gKey=codeNum^*nameNum;
	check=GetResource('Chck',128);
	gRegistered=CheckPack(kEncryptedPack,**((UInt32**)check));
	ReleaseResource(check);
	return gRegistered;
}

void EnterCode()
{
	int repeat;
	DialogPtr regDlg=GetNewDialog(133,nil,(WindowPtr)-1L);
	do{
		short hit;
		short type;
		Rect box;
		Handle item;
		repeat=false;
		DoError(SetDialogDefaultItem(regDlg,kCodeOKButton));
		DoError(SetDialogCancelItem(regDlg,kCodeCancelButton));
		GetDialogItem(regDlg,kCodeNameField,&type,&item,&box);
		SetDialogItemText(item,gPrefs.name);
		GetDialogItem(regDlg,kCodeCodeField,&type,&item,&box);
		SetDialogItemText(item,gPrefs.code);
		do{
			EventRecord event;
			WindowPtr win;
			DialogPtr dlg;
			hit=0;
			TEFromScrap();
			if(WaitNextEvent(everyEvent,&event,60,nil))
			{
				if(event.what==keyDown)
					switch((event.message&keyCodeMask)>>8)
					{
						case 0x24:
						case 0x4c:
							hit=kCodeOKButton;break;
						case 0x35:
							hit=kCodeCancelButton;break;
					}
				if(!hit){
					if(IsDialogEvent(&event))
						DialogSelect(&event,&dlg,&hit);
					else if(event.what==mouseDown)
						if(FindWindow(event.where,&win)==inDrag)
						{
							Rect rgnBBox;
							GetRegionBounds(GetGrayRgn(),&rgnBBox);
							DragWindow(win,event.where,&rgnBBox);
						}
				}
				else{
					unsigned long ticks;
					GetDialogItem(regDlg,hit,&type,&item,&box);
					HiliteControl(item,1);
					ticks=TickCount();
					while(ticks+8>TickCount());
					HiliteControl(item,0);					
				}
			}
		}while(hit!=kCodeOKButton&&hit!=kCodeCancelButton);
		if(hit==kCodeOKButton){
			AlertStdAlertParamRec alertParam={
				false,false,nil,
				"\pOK",
				nil,
				nil,
				kAlertStdAlertOKButton,
				0,
				kWindowDefaultPosition};
			GetDialogItem(regDlg,kCodeNameField,&type,&item,&box);
			GetDialogItemText(item,gPrefs.name);
			GetDialogItem(regDlg,kCodeCodeField,&type,&item,&box);
			GetDialogItemText(item,gPrefs.code);
			UpperString(gPrefs.code,false);
			StripSpaces(gPrefs.code);
			WritePrefs(false);
			if(CheckRegi())
				DoError(StandardAlert(kAlertNoteAlert,
					"\pThank you very much for registering.",
					"\pHave fun!",
					&alertParam,
					&hit));
			else{
				DoError(StandardAlert(kAlertStopAlert,
					"\pSorry, your registration code and/or name are not correct.",
					"\pPlease make sure you entered both exactly as you recieved them.",
					&alertParam,
					&hit));
				repeat=true;
			}
		}
	}while(repeat);
	DisposeDialog(regDlg);	
}


void ZKRMModal(void)
{	/*ZKRMKeyValuePair		acgData[] = { { CFSTR("mykey1"), CFSTR("myvalue1") },
										  { CFSTR("mykey2"), CFSTR("myvalue2") } };*/
	ZKRMParameters			theParameters;
	ZKRMResult				*theResult;
	ZKRMModuleStatus		theStatus;
short hit;
			AlertStdAlertParamRec alertParam={
				false,false,nil,
				"\pOK",
				nil,
				nil,
				kAlertStdAlertOKButton,
				0,
				kWindowDefaultPosition};


	// Prepare the KRM parameters
	//
	// The following information should be customised:
	//
	//		http://www.mywebsite.invalid/*.html		URLs to your web site
	//		UC9										Your vendor code    (as held by Kagi)
	//		KART_TEST_COMPLEX_ACG_UC9				Your product name   (as held by Kagi)
	//		"My Product"							Your product name   (as seen by user)
	//		<price>									Pricing information (e.g., currency-specific prices)
	//		http://order.kagi.com/?UC9				URL to your store   (as held by Kagi)
	//
	// This example also demonstrates key-value pairs being passed to the ACG system,
	// a custom purchase order number, and a string to include in the TFYP email.
	theParameters.moduleVersion   = 0;
	theParameters.moduleLanguage  = kZKRMLanguageSystem;
	theParameters.moduleOptions   = kZKRMOptionOfferWebOrder;
	theParameters.moduleUserName  = NULL;
	theParameters.moduleUserEmail = NULL;
	theParameters.productInitXML  = CFSTR(
					"<krmData vendorID=\"F6\">"
					"<transactionFailureURL></transactionFailureURL>"
					"<connectionFailureURL></connectionFailureURL>"
					"<products>"
						"<product partNo=\"\" dbName=\"Reckless_Drivin'\" supplierSKU=\"n/a\">"
							"<displayName lang=\"en\">Reckless' Drivin</displayName>"
							"<price currency=\"USD\">12.00</price>"
						"</product>"
					"</products>"
					"</krmData>");

	theParameters.productStoreURL  = CFSTR("http://order.kagi.com/?F6");
	theParameters.productTFYP      = CFSTR("KRM");
	theParameters.productPO        = CFSTR("");
	theParameters.acgKeyValueCount = 0;
//	theParameters.acgKeyValueData  = acgData;



	// Invoke the KRM
	theStatus = BeginModalKRM(&theParameters, &theResult);
	//DisplayResult(theStatus, theResult);
	if(theStatus==kZKRMModuleNoErr)
	{
		if(theResult->moduleStatus==kZKRMModuleNoErr)
		{
			if(theResult->orderStatus==kZKRMOrderSuccess)
			{
				CFStringGetPascalString(theResult->acgUserName,gPrefs.name,256,kCFStringEncodingASCII);
				CFStringGetPascalString(theResult->acgRegCode,gPrefs.code,256,kCFStringEncodingASCII);
				UpperString(gPrefs.code,false);
				StripSpaces(gPrefs.code);
				WritePrefs(false);
				if(CheckRegi())
					DoError(StandardAlert(kAlertNoteAlert,
						"\pThank you very much for registering.",
						"\pHave fun!",
						&alertParam,
						&hit));
				else
					DoError(StandardAlert(kAlertStopAlert,
						"\pSorry, your registration code and/or name are not correct.",
						"\pPlease make sure you entered both exactly as you recieved them.",
						&alertParam,
						&hit));		
			}
		}
		DisposeKRMResult(&theResult);	
	}
	else
	if(theStatus==kZKRMModuleUsedWebStore)
		Exit();
}


extern CFStringRef CSCopyUserName(Boolean useShortName)
{
	return nil;
	//return CreateCFStringFromStringResource(-16096);
}

void Register(int fullscreen)
{
	AlertStdAlertParamRec alertParam={
		false,false,nil,
		"\pOK",
		nil,
		nil,
		kAlertStdAlertOKButton,
		0,
		kWindowDefaultPosition};
	short alertHit;
	if(fullscreen)
		ScreenMode(kScreenSuspended);	
	if(!gRegistered)
	{
		DialogPtr regDlg=GetNewDialog(132,nil,(WindowPtr)-1L);
		short hit;
		DoError(SetDialogDefaultItem(regDlg,kCancelButton));
		DoError(SetDialogCancelItem(regDlg,kCancelButton));
		do{
			ModalDialog(nil,&hit);
		}while(hit>kCancelButton);
		DisposeDialog(regDlg);
		switch(hit)
		{
			case kRegAppButton:
				{
					int err;
					/*err=RR_Launch();
					if(err==memFullErr)
						DoError(StandardAlert(kAlertStopAlert,
							"\pNot enough memory to launch Register.",
							"\pPlease quit Reckless Drivin' and launch Register from the Finder.",
							&alertParam,
							&alertHit));
					else if(err==dskFulErr)
						DoError(StandardAlert(kAlertStopAlert,
							"\pNot enough free space to put Register on your drive.",
							"\pPlease delete some files.",
							&alertParam,
							&alertHit));				
					else DoError(err);*/
				}
				break;				
			case kOnlineButton:
				ZKRMModal();
				break;
			case kCodeButton:
				EnterCode();
				break;			
		}
	}
	else{
		DoError(StandardAlert(kAlertNoteAlert,
		"\pYou have already registered this game.",
		"\pThank you once again!",
		&alertParam,
		&alertHit));
	}
	if(fullscreen){
		ScreenMode(kScreenRunning);	
		ScreenUpdate(nil);
	}
}

/*			{
MANUALLY LANUCH REGISTER APP
				int err;
				FSSpec regApp;
				LaunchParamBlockRec launchParams;
				FSMakeFSSpec(0,0,"\pRegister",&regApp);
				launchParams.launchBlockID=extendedBlock;
				launchParams.launchEPBLength=extendedBlockLen;
				launchParams.launchAppSpec=&regApp;
				launchParams.launchControlFlags=launchNoFileFlags+launchContinue;
				launchParams.launchAppParameters=nil;
				err=LaunchApplication(&launchParams);
				if(err==memFullErr)
					DoError(StandardAlert(kAlertStopAlert,
						"\pNot enough memory to launch Register.",
						"\pPlease quit Reckless Drivin' and launch Register from the Finder",
						&alertParam,
						&hit));
				else if(err==fnfErr)
					DoError(StandardAlert(kAlertStopAlert,
						"\pThe Register Application could not be found.",
						"\p",
						&alertParam,
						&hit));
				else DoError(err);
			}
			break;*/
