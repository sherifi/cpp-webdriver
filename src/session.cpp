#include "session.hpp"
#include "globals.hpp"

WebdriverSessionException wse;

Session::Session() {

}

void Session::url(string url) {

  extern std::string seleniumURL;
  restio* rio = new restio();
  rio->post(seleniumURL + "/session/" + id + "/url",  "{\"url\":\"" + url + "\"}");
  rio->destroy();
}


void Session::back() {

  restio* rio = new restio();
  rio->post(seleniumURL + "/session/" + id + "/back","");
  rio->destroy();
}

void Session::forward() {

  restio* rio = new restio();
  rio->post(seleniumURL + "/session/" + id + "/forward","");
  rio->destroy();
}

void Session::refresh() {
  restio* rio = new restio();
  rio->post(seleniumURL + "/session/" + id + "/refresh","");
  rio->destroy();
}

ptree Session::execute(string script,bool async) {

  string pdata = "{\"script\":\"" + json_escape(script) + "\",\"args\": []}";
  restio* rio = new restio();
  ptree resp = rio->post(seleniumURL + "/session/" + id + ( (async) ? "/execute_async": "/execute" ) ,pdata);
  rio->destroy();
  return resp;

}

Element* Session::element(ElementQuery* eq) {
  
  restio* rio = new restio();
  std::string pdata = eq->json_encode();
  ptree resp = rio->post(seleniumURL + "/session/" + id + "/element",pdata);
  rio->destroy();

  if (resp.get<int>("status") == restio::statusmap["Success"] ) {
    return new Element(id, resp.get<string>("value.ELEMENT") );
  }
  else throw wse;
}

std::vector <Element*> Session::elements(ElementQuery* eq) {
  std::vector<Element*> result;
  std::string pdata = eq->json_encode();

  restio* rio = new restio();
  ptree resp = rio->post(seleniumURL + "/session/" + id + "/elements",pdata);
  rio->destroy();

  if (resp.get<int>("status") == restio::statusmap["Success"] ) {

     BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                   resp.get_child("value")) {
       const ptree& child = v.second;
       result.push_back(new Element(id,child.get<string>("ELEMENT")));
     }
  }
  else throw wse;
  return result;
}

Element* Session::activeElement() {
  restio* rio = new restio();
  ptree resp = rio->post(seleniumURL + "/session/" + id + "/element/active","");
  rio->destroy();

  if (resp.get<int>("status") == restio::statusmap["Success"] ) {
    return new Element(id, resp.get<string>("value.ELEMENT") );
  }
  else throw wse;
}


//################ WINDOW MANAGEMENT FCTS ##################

std::string Session::getWindowHandle() {
  /* Retrieves the handle of the current window */

  restio* rio = new restio();
  ptree resp = rio->get(seleniumURL + "/session/" + id + "/window_handle");
  rio->destroy();

  if (resp.get<int>("status") == restio::statusmap["Success"] ) {
    return resp.get<string>("value");
  }
  else throw wse;

}

int* Session::getWindowSize(string handle) {
  /* gets the size of the window identified by handle */

  int* size = (int*) malloc(2*sizeof(int));

  restio* rio = new restio();
  ptree resp = rio->get(seleniumURL + "/session/" + id + "/window/" + handle + "/size");
  return size;

}

void Session::windowSize(string handle,int width,int height) {
  /* Sets the size of the window identified by handle */

  restio* rio = new restio();
  std::string pdata = "{\"width\":" + int2string(width)   + ",";
  pdata += "\"height\":" + int2string(height) + "}";

  ptree resp = rio->post(seleniumURL + "/session/" + id + "/window/" + handle + "/size",pdata);
  
}


//################ COOKIES MANAGEMENT FCTS #################

void Session::cookie(Cookie* miam) {
  /* Sends a Cookie to the current selenium session */

  std::string pdata = miam->json_encode();

  restio* rio = new restio();
  rio->post(seleniumURL + "/session/" + id + "/cookie",pdata);
  rio->destroy();

}

void Session::deleteCookie(string cookie_name) {
  /* Deletes a Cookie from the selenium instance */

  restio* rio = new restio();
  rio->del(seleniumURL + "/session/" + id + "/cookie/" + cookie_name);
  rio->destroy();
}

std::vector <Cookie*> Session::getCookies() {
  /* Retrives all the cookies for the current selenium instance */
  
  std::vector <Cookie*> result;

  restio* rio = new restio();
  ptree resp = rio->get(seleniumURL + "/session/" + id + "/cookie");
  rio->destroy();
  if (resp.get<int>("status") == restio::statusmap["Success"] ) {

     BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                   resp.get_child("value")) {
       const ptree& child = v.second;
       Cookie* c = new Cookie("foo","bar");
       result.push_back(c);
     }
  }
  else throw wse;
  return result;

}

//################ ALERTS MANAGEMENT FCTS ##################

void Session::acceptAlert() {
  restio* rio = new restio();
  rio->post(seleniumURL + "/session/" + id + "/accept_alert","");
  rio->destroy();
}

void Session::dismissAlert() {

  restio* rio = new restio();
  rio->post(seleniumURL + "/session/" + id + "/dismiss_alert","");
  rio->destroy();
}

string Session::getAlertText() {

  restio* rio = new restio();
  ptree resp = rio->get(seleniumURL + "/session/" + id + "/alert_text");
  rio->destroy();

  if (resp.get<int>("status") == restio::statusmap["Success"] ) {
    return resp.get<string>("value");
  }
  else throw wse;
}

void Session::sendKeysToAlert(string text) {

  string pdata = "{\"text\":\"" + text  + "\"}";
  restio* rio = new restio();
  rio->post(seleniumURL + "/session/" + id + "/alert_text",pdata);
  rio->destroy();
}

string Session::getURL() {

  restio* rio = new restio();
  ptree resp = rio->get(seleniumURL + "/session/" + id + "/url");
  rio->destroy();

  if (resp.get<int>("status") == restio::statusmap["Success"] ) {
    return resp.get<string>("value");
  }
  else throw wse;
}

string Session::getScreenshot() {
  restio* rio = new restio();
  ptree resp = rio->get(seleniumURL + "/session/" + id + "/screenshot");
  rio->destroy();
  
  if (resp.get<int>("status") == restio::statusmap["Success"] ) {
    return resp.get<string>("value");
  }
  else throw wse;
}

std::vector<Log*> Session::getLogs(string logtype) {
  std::vector<Log*> result;
  std::string pdata = "{\"type\":\"" + logtype + "\"}"; 
  restio* rio = new restio();
  ptree resp = rio->post(seleniumURL + "/session/" + id + "/log",pdata);
  rio->destroy();

  if (resp.get<int>("status") == restio::statusmap["Success"] ) {

     BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                   resp.get_child("value")) {

       const ptree& child = v.second;

       Log* l = new Log();
       l->timestamp =  atoi(child.get<std::string>("timestamp").c_str());
       l->level = child.get<std::string>("level");
       l->message = child.get<std::string>("message");
       result.push_back(l);
     }
  }
  else throw wse;
  return result;
}


std::string Session::getSource() {

  restio* rio = new restio();
  ptree resp = rio->get(seleniumURL + "/session/" + id + "/source");
  rio->destroy();
  
  if (resp.get<int>("status") == restio::statusmap["Success"] ) {
    return resp.get<string>("value");
  }
  else throw wse;
}
