//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdIPWatch.hpp>
#include <System.Win.ScktComp.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Imaging.jpeg.hpp>
//---------------------------------------------------------------------------

typedef struct _PACKET
{
	String ID;            // 3 bytes
	String Host;          // unfixed length
	String separation;    // 1 byte
	String MainFunc;      // 1 byte
	String SubFunc;       // 1 byte
	int MsgLength;        // 3 bytes (String)
	String Msg;           // unfixed length
	String EndSymbol;     // 1 byte, @
}PACKET, *pPACKET;

typedef struct _CLIENT_SOCKET
{
	bool active;
	TCustomWinSocket * pSocket;
	String ClientID;
}CLIENTSOCKET, *pCLIENTSOCKET;
//---------------------------------------------------------------------------

class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TClientSocket *ClientSocket1;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Listen1;
	TMenuItem *Connect1;
	TMenuItem *Disconnect1;
	TMenuItem *N1;
	TMenuItem *Exit1;
	TMenuItem *Clear1;
	TMenuItem *ReceiceLog1;
	TMenuItem *Send1;
	TServerSocket *ServerSocket1;
	TIdIPWatch *IdIPWatch1;
	TTimer *Timer1;
	TMenuItem *Debug1;
	TComboBox *ComboBox1;
	TStatusBar *StatusBar1;
	TMemo *Memo1;
	TMemo *Memo2;
	TTimer *Timer2;
	TButton *Button1;
	TMemo *Memo3;
	TLabel *Label1;
	TButton *Button2;
	TButton *Button3;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TMemo *Memo4;
	TMemo *Memo5;
	TMemo *Memo6;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TButton *Button4;
	TLabel *Label8;
	TButton *Button5;
	TImage *Image1;
	TImage *Image3;
	TImage *Image2;
	TMemo *Memo7;
	TButton *Button6;
	TButton *Button7;

	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Listen1Click(TObject *Sender);
	void __fastcall ServerSocket1Accept(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ReceiceLog1Click(TObject *Sender);
	void __fastcall Send1Click(TObject *Sender);
	void __fastcall Connect1Click(TObject *Sender);
	void __fastcall Disconnect1Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall ClientSocket1Connect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket1Disconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket1Read(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall ClientSocket1Error(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
	void __fastcall ServerSocket1ClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall Memo1Change(TObject *Sender);
	void __fastcall ServerSocket1ClientError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall ServerSocket1ClientRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall Timer2Timer(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Receive1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);



private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);

	bool CheckClients(TCustomWinSocket *);
	String AssemblePacket(PACKET pkg);
	pPACKET DisassemblePacket(String);
	void ShowPacket(PACKET);
	void ShowPacket(pPACKET);
	void ParsePacket(pPACKET);
	void ReflashClientList();

	void dis();
    void pic();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
