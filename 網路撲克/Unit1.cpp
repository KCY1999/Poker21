//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <time.h>
#include <JPEG.HPP>

#include "Unit1.h"
#include "Unit2.h"

#define POKER_MAX 52 //maximum of poker
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

String strHost;
bool blnServer;
String newClient="";
String ClientHostName="";
String ServerName="";
bool fgConnectState;
TImage *img[5][5];
int clientNum, cnum, ati=1, LX=52, py, top=0, ds=99, np=0, exc=0, ck=5, topp=0;
//連線編號,玩家編號,玩家狀態,牌數,玩家,抽牌數,斷線編號,剩餘人數,連線人數,倒數,詢問次數,抽牌數2
TStringList *sList = new TStringList();
int a[POKER_MAX],CLIENT_MAX = 3; //maximum of client
String str=""; //牌
String dsh="",dshp="";//分割
bool dct[3];
bool start = false;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	time_t t;
	srand((unsigned)time(&t));

	fgConnectState=false;
	Disconnect1->Enabled=false;
	Form1->Caption="Local host: "+IdIPWatch1->LocalIP();

	for(int i=0;i<4;i++)
	{
		for(int j=0;j<5;j++)
		{
            img[i][j]=NULL;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Listen1Click(TObject *Sender)
{
	PACKET pkg;
	String strPkg;

	Form1->Caption="I am a server"+IdIPWatch1->LocalIP();
	Label6->Visible = true;
	Label7->Visible = true;

	Listen1->Checked =! Listen1->Checked;

	ComboBox1->Clear();

	if(Listen1->Checked)
	{
		ClientSocket1->Active = false;
		try
		{
			ServerSocket1->Active = true;
		}
		catch(...)
		{
			Listen1->Checked=false;
			ShowMessage("Be a server failure.");
			return;
		}

		ServerName="Server-"+IdIPWatch1->LocalIP();
		StatusBar1->SimpleText="Status: Chat Server Listening...";
		Connect1->Enabled=false;
		Form2->Memo1->Lines->Add("Server: "+ServerName+", socket:"+
								 IntToStr(ServerSocket1->Socket->SocketHandle));
		ComboBox1->Enabled=true;
	}
	else
	{
		if(ServerSocket1->Active)
		{
			pkg.ID="PON";
			pkg.MainFunc=1;
			pkg.SubFunc=2;
			pkg.MsgLength=0;
			pkg.Msg="";
			pkg.separation="@";
			pkg.EndSymbol="#";
			pkg.Host=ServerName;

			strPkg=AssemblePacket(pkg);
			for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
			{
				ServerSocket1->Socket->Connections[i]->SendText(strPkg);
			}
			ServerSocket1->Active=false;
		}
		Connect1->Enabled=true;
		StatusBar1->SimpleText="Status: Chat Server Close";
	}
	blnServer = true;
	Form1->Tag=0;
	Label1->Caption ="莊家";
}
//---------------------------------------------------------------------------

String TForm1::AssemblePacket(PACKET pkg)
{
	String str="",tmp;
	//-----檔頭-----
	if(pkg.ID.Length()!=3)
	{
		ShowMessage("檔頭錯誤");
		return NULL;
	}
	else
	{
		str+=pkg.ID;
	}
	//-----Host-----
	if(pkg.Host.Length()==0)
	{
		//ShowMessage("Host錯誤");
		return NULL;
	}
	else
	{
		str+=pkg.Host;
		str+=pkg.separation;
	}
	//-----Main Function-----
	str+=pkg.MainFunc;	
	//-----Sub Function-----
	str+=pkg.SubFunc;
	//-----Msg Length-----
	tmp=IntToStr(pkg.MsgLength);
	if(tmp.Length()>3)
	{
		ShowMessage("msg長度紀錄錯誤");
			return NULL;
    }
	else
	{
		for(int i=0;i<3-tmp.Length();i++)
			str+="0";

		str+=tmp;
	}
	//-----Message-----
	
	if(pkg.MsgLength!=pkg.Msg.Length())
	{
		ShowMessage("msg長度錯誤");
			return NULL;
	}
	else
	{
		str+=pkg.Msg;
		str+=pkg.EndSymbol;
	}
	return str;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ServerSocket1Accept(TObject *Sender, TCustomWinSocket *Socket)

{
   fgConnectState=true;
   Memo2->Visible=true;
   ReflashClientList() ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ReceiceLog1Click(TObject *Sender)
{
	Memo1->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Send1Click(TObject *Sender)
{
	Memo2->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Connect1Click(TObject *Sender)
{
	Form1->Caption="I am a client";
	Label6->Visible = false;
	Label7->Visible = false;

	if(ClientSocket1->Active)
		ClientSocket1->Active=false;

	strHost="127.0.0.1";
	if(InputQuery("Chat Conncetion", "Chat Server IP", strHost))
	{
		if(strHost.Length() > 0)
		{
			ClientSocket1->Host=strHost;
			ClientSocket1->Active=true;

			Listen1->Checked=false;

			blnServer=false;
			ComboBox1->Enabled=false;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Disconnect1Click(TObject *Sender)
{
	PACKET pkg;
	String strPkg;
	if(!blnServer)        //  7.4 mdf
	{
		pkg.ID="PON";
		pkg.Host=ClientHostName;

		pkg.separation="@";
		pkg.MainFunc=1; //傳送資料
		pkg.SubFunc=2;
		pkg.MsgLength=1;
		pkg.Msg = cnum;
		pkg.EndSymbol="#";
		strPkg=AssemblePacket(pkg);

		ClientSocket1->Socket->SendText(strPkg);

		Button2->Visible=false;
		Button3->Visible=false;
	}
	Sleep(100);
	dis();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	ck--;
	Label8->Caption = IntToStr(ck);

	PACKET pkg;
	String strPkg;

	if(ck==0)
	{
		ck+=5;
		if(!blnServer)
		{
			pkg.ID="PON";
			pkg.Host=ClientHostName;

			pkg.separation="@";
			pkg.MainFunc=2; //傳送資料
			pkg.SubFunc=2;
			pkg.MsgLength = 1;
			pkg.Msg = cnum;
			pkg.EndSymbol="#";
			strPkg=AssemblePacket(pkg);

			ClientSocket1->Socket->SendText(strPkg);

			Button2->Visible=false;
			Button3->Visible=false;
			Timer1->Enabled = false;
			Label8->Visible = false;
		}
		/*if(blnServer)
		{
			pkg.ID="PON";
			pkg.Host=IdIPWatch1->LocalIP();
			pkg.separation="@";
			pkg.MainFunc=2;
			pkg.SubFunc=0;
			pkg.MsgLength = 0;
			pkg.Msg = "";
			pkg.EndSymbol="#";
			strPkg=AssemblePacket(pkg);

			ServerSocket1->Socket->Connections[msg]->SendText(strPkg);

        } */
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ClientSocket1Connect(TObject *Sender, TCustomWinSocket *Socket)
{
	StatusBar1->SimpleText="Status: Connect to" + Socket->RemoteHost;
	fgConnectState=true;
	Memo2->Clear();
	Memo2->Visible=true;

	Listen1->Enabled=false;
	Connect1->Enabled=false;
	Disconnect1->Enabled=true;

	ClientSocket1->ClientType=ctNonBlocking;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ClientSocket1Disconnect(TObject *Sender, TCustomWinSocket *Socket)
{
	StatusBar1->SimpleText="server disconnect";

	fgConnectState=false;
	Memo2->Visible=false;

	Listen1->Enabled=true;
	Disconnect1->Enabled=false;
	Connect1->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ClientSocket1Read(TObject *Sender, TCustomWinSocket *Socket)
{
	pPACKET pPkg;
	String strPacket;

	if(!fgConnectState)
	{
		return;
    }

	strPacket=Socket->ReceiveText();
	Form2->Memo1->Lines->Add("["+Time()+"]"+strPacket);

	pPkg=DisassemblePacket(strPacket);
	if(pPkg==NULL)
	{
	   Memo1->Lines->Add("1Packet error");
	   return;
	}

	ParsePacket(pPkg);
	Memo1->Lines->Add(strPacket);
}
//---------------------------------------------------------------------------

pPACKET TForm1::DisassemblePacket(String strPkg)
{
   PACKET tmpPkg;
   pPACKET pPkg;
   int separator;
   String strTmp;

   Form2->Memo1->Lines->Add("===== Diassemble packet ====");

   if(strPkg[strPkg.Length()]!='#')
   {
	  Form2->Memo1->Lines->Add("Packet error 1");
	  return NULL;
   }

   if(strPkg.Pos("@")<=0)
   {
	  Form2->Memo1->Lines->Add("Packet error 2");
	  return NULL;
   }

   if(strPkg.SubString(1,3)!="PON")
   {
	  Form2->Memo1->Lines->Add("Packet error 3");
	  return NULL;
   }

   //---------------------------------------
   tmpPkg.ID=strPkg.SubString(1,3);

   separator= strPkg.Pos("@");
   tmpPkg.Host=strPkg.SubString(4,separator-4);
   tmpPkg.separation=strPkg.SubString(separator,1);

   tmpPkg.MainFunc=strPkg.SubString(separator+1,1);
   tmpPkg.SubFunc=strPkg.SubString(separator+2,1);

   //-----------------------------------------
   strTmp=strPkg.SubString(separator+3,3);
   try{
	   tmpPkg.MsgLength=StrToInt(strTmp);
   }
   catch(...)
   {
	  Form2->Memo1->Lines->Add("Packet error 4");
	  return NULL;
   }

   if(tmpPkg.MsgLength<0)
   {
	  Form2->Memo1->Lines->Add("Packet error 5");
	  return NULL;
   }

   strTmp=strPkg.SubString(separator+6,strPkg.Length()-separator-6);

   if(strTmp.Length()!=tmpPkg.MsgLength)
   {
	  Form2->Memo1->Lines->Add("Packet error 6");
	  //Memo4->Lines->Add(strPkg);
	  return NULL;
   }
   else
   {
		tmpPkg.Msg=strTmp;
   }

   pPkg = new PACKET;
   pPkg->ID=tmpPkg.ID;
   pPkg->Host=tmpPkg.Host;
   pPkg->separation=tmpPkg.separation;
   pPkg->MainFunc=tmpPkg.MainFunc;
   pPkg->SubFunc=tmpPkg.SubFunc;
   pPkg->MsgLength=tmpPkg.MsgLength;
   pPkg->Msg=tmpPkg.Msg;
   pPkg->EndSymbol=tmpPkg.EndSymbol;

   return pPkg;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender)
{
	ServerSocket1->Close();
	ClientSocket1->Close();

	Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ClientSocket1Error(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode)
{
	Memo2->Lines->Add("Error:"+strHost);
	ErrorCode = 0;
}
//---------------------------------------------------------------------------

void TForm1::ParsePacket(pPACKET pPkg)
{
	int mFunc,sFunc,msg;

	PACKET pkg;
	String strPkg;
	String srtClient;
	String strPacket;

	mFunc=StrToInt(pPkg->MainFunc);
	sFunc=StrToInt(pPkg->SubFunc);

	switch(mFunc)
	{
		case 1:
				switch(sFunc)
				{
					case 1: //new connection
							ClientHostName=pPkg->Msg;
							Form2->Memo1->Lines->Add("New name:"+ClientHostName);
							Form1->Caption=ClientHostName;
							sList->Delimiter = '-'; // 設定要分割的字元
							sList->DelimitedText = ClientHostName;
							cnum = StrToInt(sList->Strings[1]);
							Timer2->Enabled = true;
							Label1->Caption="玩家"+IntToStr(cnum);

							break;

					case 2: //Disconnect
							msg=StrToInt(pPkg->Msg);
							if(start == true)
								CLIENT_MAX--;

							Form2->Memo1->Lines->Add("["+Time()+"] Client'"+pPkg->Host+"'requests to disconnect.");
							ds = msg;
							dct[ds-1]=false;

							if(blnServer) //7.4 mdf
							{
								np++;
								ShowMessage("玩家"+IntToStr(msg)+"已斷線");
								Label7->Caption = IntToStr(clientNum-np);

								pkg.ID="PON";
								pkg.Host=IdIPWatch1->LocalIP();
								pkg.separation="@";
								pkg.MainFunc=1;
								pkg.SubFunc=2;
								pkg.MsgLength = 1;
								pkg.Msg = msg;
								pkg.EndSymbol="#";
								strPkg=AssemblePacket(pkg);

								for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
									ServerSocket1->Socket->Connections[i]->SendText(strPkg);
							}
							else
							{
								Form2->Memo1->Lines->Add("["+Time()+"] Server'"+pPkg->Host+"'disconnected.");
								ShowMessage("玩家"+IntToStr(msg)+"已斷線");
							}
							break;

					case 3:
							sList->Delimiter = ','; // 設定要分割的字元
							sList->DelimitedText = pPkg->Msg;
							Label2->Caption = "1";
							Image1->Visible = true;
							Image2->Visible = true;
							Image3->Visible = true;

							if(cnum==1)
							{
								img[cnum][0]=new TImage(this);
								img[cnum][0]->Top=69;
								img[cnum][0]->Left=270;
								img[cnum][0]->Parent=this;
								img[cnum][0]->Picture->LoadFromFile(sList->Strings[cnum]+".jpg");
								img[cnum][0]->Stretch = true;

								Memo3->Lines->Add(sList->Strings[cnum]);
							}
							if(cnum==2)
							{
								img[cnum][0]=new TImage(this);
								img[cnum][0]->Top=217;
								img[cnum][0]->Left=10;
								img[cnum][0]->Parent=this;
								img[cnum][0]->Picture->LoadFromFile(sList->Strings[cnum]+".jpg");
								img[cnum][0]->Stretch = true;

								Memo4->Lines->Add(sList->Strings[cnum]);
							}
							if(cnum==3)
							{
								img[cnum][0]=new TImage(this);
								img[cnum][0]->Top=358;
								img[cnum][0]->Left=270;
								img[cnum][0]->Parent=this;
								img[cnum][0]->Picture->LoadFromFile(sList->Strings[cnum]+".jpg");
								img[cnum][0]->Stretch = true;
;
								Memo5->Lines->Add(sList->Strings[cnum]);
							}
							break;
				}
				break;

		case 2:
				switch(sFunc)
				{
					case 0:
							Memo1->Lines->Add("["+pPkg->Host+"]:"+pPkg->Msg);

                            Label8->Visible = true;
							Button2->Visible = true;
							Button3->Visible = true;
							Timer1->Enabled = true;

							break;
					case 1:
							topp++;
							msg=StrToInt(pPkg->Msg);
							dsh=pPkg->Msg;
							dshp=pPkg->Msg;
                            Timer1->Enabled = false;

							sList->Delimiter = ','; // 設定要分割的字元
							sList->DelimitedText = pPkg->Msg;

							if(!blnServer)
							{
								dshp=dshp.SubString(1,1);           //玩家
								dsh=dsh.SubString(2,dsh.Length()-1);//牌
								if(dshp==1)
								{
									img[cnum][top]=new TImage(this);
									img[cnum][top]->Top=69;
									img[cnum][top]->Left=270+(topp*15);
									img[cnum][top]->Parent=this;
									img[cnum][top]->Picture->LoadFromFile(dsh+".jpg");
									img[cnum][top]->Stretch = true;
									Memo3->Lines->Add(dsh);
								}
								else if(dshp==2)
								{
									img[cnum][top]=new TImage(this);
									img[cnum][top]->Top=217;
									img[cnum][top]->Left=10+(topp*15);
									img[cnum][top]->Parent=this;
									img[cnum][top]->Picture->LoadFromFile(dsh+".jpg");
									img[cnum][top]->Stretch = true;
									Memo4->Lines->Add(dsh);
								}
								else if(dshp==3)
								{
									img[cnum][top]=new TImage(this);
									img[cnum][top]->Top=358;
									img[cnum][top]->Left=270+(topp*15);
									img[cnum][top]->Parent=this;
									img[cnum][top]->Picture->LoadFromFile(dsh+".jpg");
									img[cnum][top]->Stretch = true;
									Memo5->Lines->Add(dsh);
								}
								else
								{
									img[4][top]=new TImage(this);
									img[4][top]->Top=217;
									img[4][top]->Left=520+(topp*15);
									img[4][top]->Parent=this;
									img[4][top]->Picture->LoadFromFile(dsh+".jpg");
									img[4][top]->Stretch = true;
									Memo6->Lines->Add(dsh);
								}
								if(cnum==dshp)
								{
									ck=5;
									Timer1->Enabled = true;
								}
								return;
							}

							pkg.ID="PON";
							pkg.Host=IdIPWatch1->LocalIP();
							pkg.separation="@";
							pkg.MainFunc=2; //傳送資料
							pkg.SubFunc=1;
							pkg.MsgLength = IntToStr(msg).Length()+IntToStr(a[LX-1]).Length();
							pkg.Msg = IntToStr(msg)+IntToStr(a[LX-1]);
							pkg.EndSymbol="#";
							strPkg=AssemblePacket(pkg);
							if (blnServer)
							{
								for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
										ServerSocket1->Socket->Connections[i]->SendText(strPkg);
								ck=5;
                                Timer1->Enabled = true;
							}

							if(IntToStr(msg)==1)
							{
								img[cnum][top]=new TImage(this);
								img[cnum][top]->Top=69;
								img[cnum][top]->Left=270+(topp*15);
								img[cnum][top]->Parent=this;
								img[cnum][top]->Picture->LoadFromFile(IntToStr(a[LX-1])+".jpg");
								img[cnum][top]->Stretch = true;
								Memo3->Lines->Add(IntToStr(a[LX-1]));
							}
							else if(IntToStr(msg)==2)
							{

								img[cnum][top]=new TImage(this);
								img[cnum][top]->Top=217;
								img[cnum][top]->Left=10+(topp*15);
								img[cnum][top]->Parent=this;
								img[cnum][top]->Picture->LoadFromFile(IntToStr(a[LX-1])+".jpg");
								img[cnum][top]->Stretch = true;
								Memo4->Lines->Add(IntToStr(a[LX-1]));
							}
							else if(IntToStr(msg)==3)
							{
								img[cnum][top]=new TImage(this);
								img[cnum][top]->Top=358;
								img[cnum][top]->Left=270+(topp*15);;
								img[cnum][top]->Parent=this;
								img[cnum][top]->Picture->LoadFromFile(IntToStr(a[LX-1])+".jpg");
								img[cnum][top]->Stretch = true;
								Memo5->Lines->Add(IntToStr(a[LX-1]));
							}
							else
							{
								Memo6->Lines->Add(IntToStr(a[LX-1]));
							}

							Form2->Memo1->Lines->Add(LX);
							LX--;
							break;

					case 2:
					{
							int CN=0;
							CN = ServerSocket1->Socket->ActiveConnections;
                            topp=0;

							msg=StrToInt(pPkg->Msg);
							Timer1->Enabled = false;

							if(msg<3 && ds==99)
							{
								pkg.ID="PON";
								pkg.Host=IdIPWatch1->LocalIP();
								pkg.separation="@";
								pkg.MainFunc=2;
								pkg.SubFunc=0;
								pkg.MsgLength = 0;
								pkg.Msg = "";
								pkg.EndSymbol="#";
								strPkg=AssemblePacket(pkg);

								ServerSocket1->Socket->Connections[msg]->SendText(strPkg);

								ck=5;
								Timer1->Enabled = true;

								Label2->Caption = IntToStr(StrToInt(msg+1));

								Sleep(500);

								pkg.ID="PON";
								pkg.Host=IdIPWatch1->LocalIP();
								pkg.separation="@";
								pkg.MainFunc=2;
								pkg.SubFunc=3;
								pkg.MsgLength = 1;
								pkg.Msg = msg;
								pkg.EndSymbol="#";
								strPkg=AssemblePacket(pkg);

								for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
									ServerSocket1->Socket->Connections[i]->SendText(strPkg);
							}
							if(msg>=3 && ds==99)
							{
								pkg.ID="PON";
								pkg.Host=IdIPWatch1->LocalIP();
								pkg.separation="@";
								pkg.MainFunc=2;
								pkg.SubFunc=3;
								pkg.MsgLength = 1;
								pkg.Msg = 4;
								pkg.EndSymbol="#";
								strPkg=AssemblePacket(pkg);

								for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
									ServerSocket1->Socket->Connections[i]->SendText(strPkg);

								Label2->Caption = "莊家";
								Button2->Visible = true;
								Button3->Visible = true;
							}
//=======================================================================================================

							if(ds!=99)
							{
								pkg.ID="PON";
								pkg.Host=IdIPWatch1->LocalIP();
								pkg.separation="@";
								pkg.MainFunc=2;
								pkg.SubFunc=0;
								pkg.MsgLength = 0;
								pkg.Msg = "";
								pkg.EndSymbol="#";
								strPkg=AssemblePacket(pkg);
//=======================================================================================================

								if(dct[msg-1]==true && ds<3)
								{
									if(CN>1)
									{
										if(msg < CN)
										{
											ServerSocket1->Socket->Connections[CN-1]->SendText(strPkg);
										}
										if(msg == CN) //如果超出陣列
											ServerSocket1->Socket->Connections[CN-1]->SendText(strPkg);
										if(msg > CN) //如果超出陣列
										{
                                        	ds = 99;
											pkg.ID="PON";
											pkg.Host=IdIPWatch1->LocalIP();
											pkg.separation="@";
											pkg.MainFunc=2;
											pkg.SubFunc=3;
											pkg.MsgLength = 1;
											pkg.Msg = 4;
											pkg.EndSymbol="#";
											strPkg=AssemblePacket(pkg);

											for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
												ServerSocket1->Socket->Connections[i]->SendText(strPkg);

											Label2->Caption = "莊家";
											Button2->Visible = true;
											Button3->Visible = true;
										}
									}
									else if(CN == 1)
									{
										pkg.ID="PON";
										pkg.Host=IdIPWatch1->LocalIP();
										pkg.separation="@";
										pkg.MainFunc=2;
										pkg.SubFunc=3;
										pkg.MsgLength = 1;
										pkg.Msg = 4;
										pkg.EndSymbol="#";
										strPkg=AssemblePacket(pkg);

										for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
											ServerSocket1->Socket->Connections[i]->SendText(strPkg);

										Label2->Caption = "莊家";
										Button2->Visible = true;
										Button3->Visible = true;
                                    }
								}
//=======================================================================================================
								else if(!dct[msg-1] && ds<3)
								{
									CN = ServerSocket1->Socket->ActiveConnections;

									if(CN==1)
									{
										if(ds==2)
										{
                                        	pkg.ID="PON";
											pkg.Host=IdIPWatch1->LocalIP();
											pkg.separation="@";
											pkg.MainFunc=2;
											pkg.SubFunc=3;
											pkg.MsgLength = 1;
											pkg.Msg = 4;
											pkg.EndSymbol="#";
											strPkg=AssemblePacket(pkg);

											for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
												ServerSocket1->Socket->Connections[i]->SendText(strPkg);

											Label2->Caption = "莊家";
											Button2->Visible = true;
											Button3->Visible = true;
                                        }
										else
										ServerSocket1->Socket->Connections[0]->SendText(strPkg);
									}
									else if(CN!=1)
									{
										ShowMessage("2-3");
										ServerSocket1->Socket->Connections[msg-1]->SendText(strPkg);
									}
								}
//=======================================================================================================
								else if(dct[msg-1] && ds==3)    //換莊家
								{
									if(msg < CN)
									{
										ServerSocket1->Socket->Connections[CN-1]->SendText(strPkg);
									}
									if(msg == 2||msg == CN)
									{
										pkg.ID="PON";
										pkg.Host=IdIPWatch1->LocalIP();
										pkg.separation="@";
										pkg.MainFunc=2;
										pkg.SubFunc=3;
										pkg.MsgLength = 1;
										pkg.Msg = 4;
										pkg.EndSymbol="#";
										strPkg=AssemblePacket(pkg);

										for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
												ServerSocket1->Socket->Connections[i]->SendText(strPkg);


										Label2->Caption = "莊家";
										Button2->Visible = true;
										Button3->Visible = true;
									}
								}
								else if(!dct[msg-1] && ds==3)
								{
									pkg.ID="PON";
									pkg.Host=IdIPWatch1->LocalIP();
									pkg.separation="@";
									pkg.MainFunc=2;
									pkg.SubFunc=3;
									pkg.MsgLength = 1;
									pkg.Msg = 4;
									pkg.EndSymbol="#";
									strPkg=AssemblePacket(pkg);

									for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
										ServerSocket1->Socket->Connections[i]->SendText(strPkg);


									Label2->Caption = "莊家";
									Button2->Visible = true;
									Button3->Visible = true;
                                }
								ck=5;
								Timer1->Enabled = true;
							}
//=======================================================================================================

							Sleep(100);
							break;
                    }

					case 3:
							py = StrToInt(pPkg->Msg)+1;
							topp=0;
							if(py<4)
							{
								Label2->Caption=IntToStr(py);
							}
							else
							{
								Label2->Caption = "莊家";
							}
				}
				break;
		case 3:
				if(mFunc==3 && sFunc==0)
				{
					pkg.ID="PON";
					if(blnServer)
						pkg.Host=IdIPWatch1->LocalIP();
					else
						pkg.Host=ClientHostName;

						pkg.separation="@";
						pkg.MainFunc=3;
						pkg.SubFunc=1;
						pkg.MsgLength = 1;
						pkg.Msg = cnum;
						pkg.EndSymbol="#";
						strPkg=AssemblePacket(pkg);
						if (blnServer)
						{
							for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
									ServerSocket1->Socket->Connections[i]->SendText(strPkg);
						}
						else // Client
							ClientSocket1->Socket->SendText(strPkg) ;
					Memo2->Lines->Add(strPkg);
				}
				if(mFunc==3 && sFunc==2)
				{
					Memo7->Visible=true;

					PACKET pkg;
					String strPkg;
					if(!blnServer)        //  7.4 mdf
					{
						pkg.ID="PON";
						pkg.Host=ClientHostName;

						pkg.separation="@";
						pkg.MainFunc=1; //傳送資料
						pkg.SubFunc=2;
						pkg.MsgLength=1;
						pkg.Msg = cnum;
						pkg.EndSymbol="#";
						strPkg=AssemblePacket(pkg);

						ClientSocket1->Socket->SendText(strPkg);

						Button2->Visible=false;
						Button3->Visible=false;
					}
					Sleep(100);
					dis();
                }
		default:
				break;
    }
}
//---------------------------------------------------------------------------

void TForm1::ReflashClientList()
{
	ComboBox1->Clear();

	if(ServerSocket1->Socket->ActiveConnections>1)
	{
		ComboBox1->Items->Add("All");
	}

	for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
	{
		ComboBox1->Items->Add(ServerSocket1->Socket->Connections[i]->RemoteAddress+"-"+
							  IntToStr(ServerSocket1->Socket->Connections[i]->SocketHandle));
	}

	ComboBox1->ItemIndex=0;
	ComboBox1->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ServerSocket1ClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{
	Form2->Memo1->Lines->Add("'"+Socket->RemoteAddress+"-"+IntToStr(Socket->SocketHandle)+"'disconnect");
	StatusBar1->SimpleText="Status:Listening...";

	if(ServerSocket1->Socket->ActiveConnections==1)
	{
		fgConnectState=false;
		Memo2->Visible=false;
	}
	ComboBox1->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Memo1Change(TObject *Sender)
{

	ReflashClientList();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ServerSocket1ClientError(TObject *Sender, TCustomWinSocket *Socket,
		  TErrorEvent ErrorEvent, int &ErrorCode)
{
	StatusBar1->SimpleText="Error from Client Socket.";

	switch( ErrorCode )
    {
	  case 10053: Socket->Close();
      case 10054: Socket->Close();
                  break;
	}

 	ErrorCode = 0;
	Sleep(300);
	StatusBar1->SimpleText="Error from Client scoket.";
	ReflashClientList();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ServerSocket1ClientRead(TObject *Sender, TCustomWinSocket *Socket)
{
	pPACKET pPkg;
	String strPacket;

	strPacket=Socket->ReceiveText();
	Form2->Memo1->Lines->Add("["+Time()+"]"+strPacket);
	pPkg=DisassemblePacket(strPacket);

	if(pPkg==NULL)
	{
		Memo1->Lines->Add("disconnect");
		return;
	}
	ParsePacket(pPkg);
   	Memo1->Lines->Add(strPacket);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{
	PACKET pkg;
	String strPkg;

	clientNum = ServerSocket1->Socket->ActiveConnections;

	if(clientNum>CLIENT_MAX)
	{
		Form2->Memo1->Lines->Add("已達連線最大數量");
		Socket->Close();
		return;
	}
	StatusBar1->SimpleText="Status:Connect from"+Socket->RemoteHost+"socket ID:"+IntToStr(Socket->SocketHandle);

	Sleep(500);

	if(!CheckClients(Socket))
	{
		pkg.ID="PON";
		pkg.Host=ServerName;
		pkg.separation="@";
		pkg.MainFunc=1;
		pkg.SubFunc=1;
		pkg.MsgLength=newClient.Length();
		pkg.Msg=newClient;
		pkg.EndSymbol="#";
		strPkg=AssemblePacket(pkg);
		Sleep(500);

		ServerSocket1->Socket->Connections[clientNum-1]->SendText(strPkg);
	}
	if(clientNum+=1)
	{
		Label7->Caption = IntToStr(clientNum);
		if(clientNum==4)
		{
			Timer2->Enabled = true;
			Button1->Visible = true;
			exc+=4;
		}
	}
}
//---------------------------------------------------------------------------
bool TForm1::CheckClients(TCustomWinSocket *Socket)
{
	bool fg;
	//int sHnd;
	String hostIP;

	hostIP=Socket->RemoteAddress;
	//sHnd=Socket->SocketHandle;
	//if(ds==99)
	newClient=hostIP+"-"+IntToStr(clientNum);
	/*if(ds!=99)
	newClient=hostIP+"-"+IntToStr(ds);*/
	Form2->Memo1->Lines->Add("New connection"+newClient);
	Form2->Memo1->Lines->Add("Connection number:"+
							 IntToStr(ServerSocket1->Socket->ActiveConnections));

	fg=false;
	return fg;
}
//---------------------------------------------------------------------------
void TForm1::dis()
{
	String newClient="";
	String ClientHostName="";
	String ServerName="";
	clientNum=0, cnum=0, ati=1, LX=52, py=0, top=0, ds=99, np=0, exc=0, ck=5, topp=0;
	CLIENT_MAX = 3;
	str="";
	dsh="",dshp="";

	PACKET pkg;
	String strPkg;

	if(blnServer)
	{
		return;
	}

	pkg.ID="PON";
	pkg.MainFunc=1;
	pkg.SubFunc=2;
	pkg.MsgLength=0;
	pkg.Msg="";
	pkg.separation="#";
	pkg.EndSymbol="@";
	pkg.Host=ClientHostName;

	strPkg=AssemblePacket(pkg);

	ClientSocket1->Socket->SendText(strPkg);
	ClientSocket1->Active=false;

	Memo2->Lines->Add("["+Time()+"] Disconnect.");
	Listen1->Checked=false;
	Connect1->Checked=false;
	Disconnect1->Checked=false;

	fgConnectState=false;
	Memo2->Visible=false;

	StatusBar1->SimpleText="Status: Disconnect";
	Form1->Caption="";
}

void __fastcall TForm1::Timer2Timer(TObject *Sender)     //握手
{
	Label5->Caption=IntToStr(ds);

	PACKET pkg;
	String strPkg;

	pkg.ID="PON";
	if(blnServer)
		pkg.Host=IdIPWatch1->LocalIP();
	else
		pkg.Host=ClientHostName;

		pkg.separation="@";
		pkg.MainFunc=3; //傳送資料
		pkg.SubFunc=0;
		pkg.MsgLength = 0;
		pkg.Msg = "";
		pkg.EndSymbol="#";
		strPkg=AssemblePacket(pkg);

		for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
			ServerSocket1->Socket->Connections[i]->SendText(strPkg);

		//Memo2->Lines->Add(strPkg);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	String str="";
	int k,tmp,pos;
	Label8->Visible = true;
	Timer1->Enabled = true;
	ck = 6;
	start = true;
	Image1->Visible = true;
	Image2->Visible = true;
	Image3->Visible = true;


	for(int i=0;i<3;i++)
	{
		dct[i]=true;
	}

	srand((unsigned)time(NULL));
	a[0] = POKER_MAX;

	for(int i=1;i<POKER_MAX;i++)
	{
		a[i] = i;
	}
	//洗牌
	for(int i=0;i<POKER_MAX;i++)
	{
		pos=(POKER_MAX-1) * rand() / RAND_MAX;
		tmp=a[i];
		a[i]=a[pos];
		a[pos]=tmp;
	}

	for(int i=0;i<clientNum+1;i++)
	{
		str+=IntToStr(a[LX-1])+",";
		LX--;
	}

	PACKET pkg;
	String strPkg;

	pkg.ID="PON";
	if(blnServer)
		pkg.Host=IdIPWatch1->LocalIP();
	else
		pkg.Host=ClientHostName;

		pkg.separation="@";
		pkg.MainFunc=1; //傳送資料
		pkg.SubFunc=3;
		pkg.MsgLength = str.Length();
		pkg.Msg = str;
		pkg.EndSymbol="#";
		strPkg=AssemblePacket(pkg);
		if (blnServer)
		{
			for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
					ServerSocket1->Socket->Connections[i]->SendText(strPkg);
		}

	Button1->Visible=false;

	Sleep(500);

	pkg.ID="PON";
	pkg.Host=IdIPWatch1->LocalIP();
	pkg.separation="@";
	pkg.MainFunc=2;
	pkg.SubFunc=0;
	pkg.MsgLength = 0;
	pkg.Msg = "";
	pkg.EndSymbol="#";
	strPkg=AssemblePacket(pkg);

	ServerSocket1->Socket->Connections[0]->SendText(strPkg);
	Label2->Caption = "1";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	PACKET pkg;
	String strPkg;
	Label8->Visible = false;

	if(!blnServer)
	{
		pkg.ID="PON";
		pkg.Host=ClientHostName;

		pkg.separation="@";
		pkg.MainFunc=2; //傳送資料
		pkg.SubFunc=2;
		pkg.MsgLength = 1;
		pkg.Msg = cnum;
		pkg.EndSymbol="#";
		strPkg=AssemblePacket(pkg);

		ClientSocket1->Socket->SendText(strPkg);

		Button2->Visible=false;
		Button3->Visible=false;
	}
	if(blnServer)
	{
		Button4->Visible = true;
		Button2->Visible = false;
		Button3->Visible = false;
    }
	Timer1->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
	PACKET pkg;
	String strPkg;

	Timer1->Enabled = false;
	if(!blnServer)
	{
		if(top>3)
		{
			Button2->Visible=false;
			Button3->Visible=false;

			pkg.ID="PON";
			pkg.Host=ClientHostName;

			pkg.separation="@";
			pkg.MainFunc=2; //傳送資料
			pkg.SubFunc=2;
			pkg.MsgLength = 1;
			pkg.Msg = cnum;
			pkg.EndSymbol="#";
			strPkg=AssemblePacket(pkg);

			ClientSocket1->Socket->SendText(strPkg);
		}
		else
		{
			pkg.ID="PON";
			pkg.Host=ClientHostName;

			pkg.separation="@";
			pkg.MainFunc=2; //傳送資料
			pkg.SubFunc=1;
			pkg.MsgLength = 1;
			pkg.Msg = cnum;
			pkg.EndSymbol="#";
			strPkg=AssemblePacket(pkg);

			ClientSocket1->Socket->SendText(strPkg);

			top+=1;
		}
	}
	if(blnServer)
	{
		if(top>4)
		{
			Button4->Visible = true;
			Button2->Visible = false;
			Button3->Visible = false;
		}
		else
		{
			Sleep(100);
			pkg.ID="PON";
			pkg.Host=IdIPWatch1->LocalIP();
			pkg.separation="@";
			pkg.MainFunc=2; //傳送資料
			pkg.SubFunc=1;
			pkg.MsgLength = 1+IntToStr(a[LX-1]).Length();
			pkg.Msg = "4"+IntToStr(a[LX-1]);
			pkg.EndSymbol="#";
			strPkg=AssemblePacket(pkg);

			for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
				ServerSocket1->Socket->Connections[i]->SendText(strPkg);

			Form2->Memo1->Lines->Add(LX);

			top+=1;

			img[4][top]=new TImage(this);
			img[4][top]->Top=217;
			img[4][top]->Left=520+(top*15);
			img[4][top]->Parent=this;
			img[4][top]->Picture->LoadFromFile(IntToStr(a[LX-1])+".jpg");
			Memo6->Lines->Add(IntToStr(a[LX-1]));
			LX--;
			//Timer1->Enabled = true;
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Receive1Click(TObject *Sender)
{
	Form2->Top=Form1->Top;
	Form2->Left=Left+Form1->Width+10;
	Form2->Show();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	PACKET pkg;
	String strPkg;
	if(!blnServer)        //  7.4 mdf
	{
		pkg.ID="PON";
		pkg.Host=ClientHostName;

		pkg.separation="@";
		pkg.MainFunc=1; //傳送資料
		pkg.SubFunc=2;
		pkg.MsgLength=1;
		pkg.Msg = cnum;
		pkg.EndSymbol="#";
		strPkg=AssemblePacket(pkg);

		ClientSocket1->Socket->SendText(strPkg);

		Sleep(100);

		if(Button2->Visible==true)
		{
			pkg.ID="PON";
			pkg.Host=ClientHostName;

			pkg.separation="@";
			pkg.MainFunc=2; //傳送資料
			pkg.SubFunc=2;
			pkg.MsgLength = 1;
			pkg.Msg = cnum;
			pkg.EndSymbol="#";
			strPkg=AssemblePacket(pkg);

			ClientSocket1->Socket->SendText(strPkg);
		}
		Button2->Visible=false;
		Button3->Visible=false;
	}
	Sleep(100);
	dis();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
	Button5->Visible = true;
	start = false;

    PACKET pkg;
	String strPkg;

	pkg.ID="PON";
	pkg.MainFunc=3;
	pkg.SubFunc=2;
	pkg.MsgLength=0;
	pkg.Msg="";
	pkg.separation="@";
	pkg.EndSymbol="#";
	pkg.Host=ServerName;

	strPkg=AssemblePacket(pkg);
	for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
	{
		ServerSocket1->Socket->Connections[i]->SendText(strPkg);
	}
	ServerSocket1->Active=false;
	ShowMessage("玩家一:第二 玩家二:第三 玩家三:第四 玩家四:第一");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender)
{
	dis();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button6Click(TObject *Sender)
{
		   if(img[4][5]!=NULL)
    		return;
			img[4][5]=new TImage(this);
			img[4][5]->Top=100;
			img[4][5]->Left=100;
			img[4][5]->Parent=this;
			img[4][5]->Picture->LoadFromFile("back.jpg");

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button7Click(TObject *Sender)
{
	for(int i=1;i<=5;i++)
	{
		for(int j=0;j<5;j++)
		{
			Memo7->Lines->Add("1");
			/*if(img[i][j]==NULL)
			{
				Memo7->Lines->Add("error"+IntToStr(i)+IntToStr(j));
				//ShowMessage("遊戲結束");
				return;
			}*/
			Memo7->Lines->Add("2");
			delete img[i][j];
			Memo7->Lines->Add("3");
			img[i][j]=NULL;
			Memo7->Lines->Add("4");
		}
	}
}
//---------------------------------------------------------------------------
