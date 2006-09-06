
#include "StripChart.h"


#include <wx/arrimpl.cpp> 

#define UNITS_LABEL_SPACING 2

//the data buffer
WX_DEFINE_OBJARRAY(ChartScales);
WX_DEFINE_OBJARRAY(LogItemTypes);
WX_DEFINE_OBJARRAY(LogItemBuffer);




BEGIN_EVENT_TABLE( StripChart, wxWindow )
	EVT_PAINT( StripChart::OnPaint )
	EVT_SIZE( StripChart::OnSize )
END_EVENT_TABLE()


StripChart::StripChart(		wxWindow *parent,
							wxWindowID id,
							const wxPoint &pos,
							const wxSize &size)
							: wxWindow(parent, id, pos, size)
							
{
	if (parent){
		SetBackgroundColour(parent->GetBackgroundColour());
	}
	else{
		SetBackgroundColour(*wxBLACK);  
	}
	_dataBufferSize = DEFAULT_DATA_BUFFER_SIZE;
	_currentWidth = size.GetWidth();
	_currentHeight = size.GetHeight();
	_memBitmap = new wxBitmap(_currentWidth, _currentHeight);
}


StripChart::~StripChart(){
	delete (_memBitmap);
}


int StripChart::AddScale(ChartScale *scale){
	_chartScales.Add(scale);
	return _chartScales.size() - 1;
}

ChartScale * StripChart::GetScale(int id){
	return &_chartScales[id];	
}

int StripChart::AddLogItemType(LogItemType *logItemType){
	_logItemTypes.Add(logItemType);
	return _logItemTypes.size() - 1;	
}

LogItemType* StripChart::GetLogItemType(int id){
	return &_logItemTypes[id];	
}

void StripChart::LogData(StripChartLogItem *values){
	_dataBuffer.Add(values);
	if (_dataBuffer.size() > _dataBufferSize){
		_dataBuffer.RemoveAt(0);
	}
	Refresh();
}


void StripChart::OnSize(wxSizeEvent &event){
	Refresh();
}


void StripChart::OnPaint(wxPaintEvent &event){

	wxPaintDC old_dc(this);
	
	int w,h ;
	GetClientSize(&w,&h);
	
	if (w != _currentWidth || h != _currentHeight){
		delete (_memBitmap);
		_currentWidth = w;
		_currentHeight = h;
		_memBitmap = new wxBitmap(_currentWidth, _currentHeight);
	}
	/////////////////
	// Create a memory DC
	wxMemoryDC dc;
	dc.SelectObject(*_memBitmap);

	wxColor backColor = GetBackgroundColour();
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.Clear();
	
	DrawGrid(dc);
	DrawUnits(dc);

	unsigned int dataBufferSize = _dataBuffer.size();
	unsigned int itemTypeSize = _logItemTypes.size();
	
	for (unsigned int typeId = 0; typeId < itemTypeSize; typeId++){
		
		LogItemType itemType = _logItemTypes[typeId];
		ChartScale scale = _chartScales[itemType.scaleId];
		
		double minValue = scale.minValue;
		double maxValue = scale.maxValue;
		
		int currentX = w - dataBufferSize;
		
		dc.SetPen(*wxThePenList->FindOrCreatePen(itemType.lineColor, 1, wxSOLID));
		
		int lastX = currentX;
		int lastY;
		if (dataBufferSize > 0){
			StripChartLogItem logItem = _dataBuffer[0];
			double loggedValue = logItem[typeId];
			double percentageOfMax = (loggedValue - minValue) / (maxValue - minValue);
			lastY = h - (int)(((double)h) * percentageOfMax);
		}
		for (unsigned int i = 1; i < dataBufferSize; i++){
			StripChartLogItem logItem = _dataBuffer[i];
			double loggedValue = logItem[typeId];
			
			double percentageOfMax = (loggedValue - minValue) / (maxValue - minValue);
			
			int y = h - (int)(((double)h) * percentageOfMax);
			
			dc.DrawLine(lastX, lastY, currentX, y);
			lastX = currentX;
			lastY = y;
			currentX++;		
		}
	}

	//blit into the real DC
	old_dc.Blit(0,0,_currentWidth,_currentHeight,&dc,0,0);
	
}

void StripChart::DrawUnits(wxMemoryDC &dc){
	
	unsigned int logItemTypes = _logItemTypes.size();
	
	int leftOrientationEdge = 0;
	int rightOrientationEdge = _currentWidth - 1;
	
	
	wxFont labelFont = GetFont();
	for (unsigned int i = 0; i < logItemTypes; i++){
		
		LogItemType *logItemType = &_logItemTypes[i];
		ChartScale *scale = &_chartScales[logItemType->scaleId];
		
		ChartScale::UNITS_DISPLAY_ORIENTATION orientation = scale->displayOrientation;
		double stepInterval = scale->stepInterval;
		double minValue = scale->minValue;
		double maxValue = scale->maxValue;
		
		dc.SetPen(*wxThePenList->FindOrCreatePen(logItemType->lineColor, 1, wxSOLID));

		wxString chartLabel = logItemType->typeLabel + " ( " + scale->scaleLabel + " )";
		int labelWidth,labelHeight,descent,externalLeading;
		dc.GetTextExtent(chartLabel, &labelHeight, &labelWidth, &descent, &externalLeading, &labelFont);


		dc.SetTextForeground(logItemType->lineColor);
	
		int verticalMiddleOfChart = _currentHeight / 2;
		
		int tickFromX = 0;
		int tickToX = 0; 
		
		bool showLabels;
		showLabels = (_currentHeight > labelHeight * 2  );
		switch (orientation){
			case ChartScale::ORIENTATION_LEFT:
			{
				if (showLabels) dc.DrawRotatedText(chartLabel,leftOrientationEdge, verticalMiddleOfChart + (labelHeight / 2),90);
				leftOrientationEdge += labelWidth;
				tickFromX = leftOrientationEdge;
				leftOrientationEdge += 5;
				tickToX = leftOrientationEdge;
				dc.DrawLine(tickFromX, 0 , tickFromX, _currentHeight);
				break;
			}
			case ChartScale::ORIENTATION_RIGHT:
			{
				if (showLabels) dc.DrawRotatedText(chartLabel, rightOrientationEdge, verticalMiddleOfChart - (labelHeight / 2), 270);
				rightOrientationEdge -= labelWidth;
				tickToX = rightOrientationEdge;
				rightOrientationEdge -= 5;
				tickFromX = rightOrientationEdge;
				dc.DrawLine(tickToX, 0 , tickToX, _currentHeight);
				break;
			}	
		}
		
		
		bool labelOn = false;
		int tickLabelWidth,tickLabelHeight,tickDescent,tickExternalLeading;
		for (double tick = minValue; tick <=maxValue; tick = tick + stepInterval){
			
			double percentageOfMax = (tick - minValue) / (maxValue - minValue);
			int y = _currentHeight - (int)(((double)_currentHeight) * percentageOfMax);
			
			dc.DrawLine(tickFromX, y, tickToX, y);
			
			if (labelOn){
				wxString tickLabel;
				tickLabel.Printf("%d",(int)tick);
				dc.GetTextExtent(chartLabel, &tickLabelHeight, &tickLabelWidth, &tickDescent, &tickExternalLeading, &labelFont);
				if (showLabels){
					switch (orientation){
						case ChartScale::ORIENTATION_LEFT:
						{
							dc.DrawRotatedText(tickLabel, leftOrientationEdge, y + (tickLabelWidth / 2), 90);
							break;				
						}
						case ChartScale::ORIENTATION_RIGHT:
						{
							dc.DrawRotatedText(tickLabel, rightOrientationEdge, y - (tickLabelWidth / 2), 270);
							break;
						}	
					}
				}
			}
			labelOn = !labelOn;
		}
		switch (orientation){
			case ChartScale::ORIENTATION_LEFT:
			{
				leftOrientationEdge += (tickLabelWidth);
				break;	
			}
			case ChartScale::ORIENTATION_RIGHT:
			{
				rightOrientationEdge -= (tickLabelWidth);
				break;
			}
				
		}
	}
}

void StripChart::DrawGrid(wxMemoryDC &dc){

	dc.SetPen(*wxThePenList->FindOrCreatePen(wxColor(40,40,40), 1, wxSOLID));
	
	int width = _currentWidth;
	int height = _currentHeight;
	
	for (int x = 0; x < width; x+=20){
		dc.DrawLine(x, 0, x, height);
	}
	
	float i = 0;
	while (i < 1){
		int y = (int)(((float)height) * i);
		dc.DrawLine(0, y, width, y);
		i = i + 0.1;				
	}	
}
