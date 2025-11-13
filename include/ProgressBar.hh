
#ifndef PROGBAR_HH 
#define PROGBAR_HH 1

#include <time.h>
#include <format>

using namespace std;

#pragma once
#include <atomic>

class ProgressBar {
  public:
    
    inline static std::atomic<uint64_t> gEvtNb{0};
    
    ProgressBar(uint64_t max_events, int readout_interval=1, int bar_width=40) {
        fMaxEvents = max_events;
        fTimeInterval = readout_interval;
        fLastEvent = 0;
        time(&fLastTime);

        fBarWidth = bar_width;

        fN = 120;
        fRates.resize(fN);
        for(int i=0; i<fN; i++) {
            fRates[i] = 0;
        }
        fNextToReplace = 0;

        fCounter = 0;
        time(&fStartTime);
    }
    ~ProgressBar() {}

    void SetMaxEvents(uint64_t n) { fMaxEvents = n; }
    void SetPrintInterval(int time) { fTimeInterval = time; }
    bool Print(uint64_t event) {
        time(&fThisTime);
        bool printing = false;
        if((difftime(fThisTime,fLastTime) >= fTimeInterval) || event == (fMaxEvents-1)) {  
            printing = true;
            double percent = 1.*event/fMaxEvents;
            double rate;
            double thisRate = ((double)(event-fLastEvent))/difftime(fThisTime,fLastTime);
            if(fCounter < 2) {
               rate = thisRate;
            }
            else { 
                rate = Rate();
            }
            int timeLeftSeconds = ((double)(fMaxEvents-event))/rate;
            int timeLeftMinutes = timeLeftSeconds/60;
            int timeLeftHours   = timeLeftMinutes/60;
            fLastTime = fThisTime;
            fLastEvent = event;

            fRates[fNextToReplace] = thisRate;
            if(fNextToReplace < fN-1)
              fNextToReplace++;
            else
              fNextToReplace = 0;  

            int timeRunningSeconds = double(fThisTime-fStartTime);
            int timeRunningMinutes = timeRunningSeconds/60;
            int timeRunningHours   = timeRunningMinutes/60;


            cout << "\33[2K\r  -> [";
            for(int i=0; i<fBarWidth; i++) {
                if(i<percent*fBarWidth) cout << "=";
                else                    cout << " ";
            }

            fSSPercent.str("");
            fSSPercent.clear();
            fSSPercent << std::fixed << std::setprecision(1) << 100.0*percent;
            
            if( event == (fMaxEvents-1))
                cout << "] all done!" << flush << endl;
            else
                cout << "] processing evt " << event+1 << " / " << fMaxEvents << " (" << fSSPercent.str() << "%), " << timeLeftHours << "h " 
                     << timeLeftMinutes%60 << "m " << timeLeftSeconds%60 << "s left @ " << (int)rate << " evts/s" << flush;
                //cout << "] processing evt " << event+1 << " / " << fMaxEvents << " (" << Form("%.1f",(100*percent)) << "%), " << timeLeftHours << "h " 
                //     << timeLeftMinutes%60 << "m " << timeLeftSeconds%60 << "s left @ " << (int)rate << " evts/s" << flush;
                cout << ", " << timeRunningHours << "h " << timeRunningMinutes%60 << "m " << timeRunningSeconds%60 << "s elapsed" << flush;
            fCounter++;
        }
        return printing;
    }

    double Rate() {
        double rate = 0;
        int counter = 0;
        for(int i=0; i<fN; i++) {
            if(fRates[i] > 0) {
                rate += fRates[i];
                counter++;
            }
        }
        rate /= double(counter);
        return rate;
    }

  private:
    uint64_t fMaxEvents;
    uint64_t fLastEvent;
    time_t fLastTime;
    time_t fThisTime;
    time_t fStartTime;
    int fTimeInterval;

    int fBarWidth;

    uint64_t fCounter;

    int                     fN;
    std::vector<double>     fRates;
    int                     fNextToReplace;

    std::ostringstream      fSSPercent; 
};

#endif
