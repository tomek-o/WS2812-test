//---------------------------------------------------------------------------

#ifndef TabManagerH
#define TabManagerH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include <vector>

class TabManager
{
private:
	TabManager();
	virtual ~TabManager() {};
	TabManager(const TabManager& source) {};
	TabManager& operator=(const TabManager&);
	static TabManager instance;
	std::vector<TForm*> forms;
public:
	static TabManager& Instance() {
		return instance;
	}
	int Register(TForm *frm);
	std::vector<TForm*>& GetForms(void) {
    	return forms;
	}
};

#endif
