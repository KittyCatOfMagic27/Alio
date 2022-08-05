import docus from "./docus.json" assert {type:"json"};

let menuElements = document.getElementsByClassName("selectItem");
let content = document.getElementById('content');
let searchbar = document.getElementsByClassName("search-bar")[0];
const pages = new Map();

class Element{
  constructor(type){
    this.type = type;
    this.attr = [];
    this.children = [];
    this.text = "";
    return this;
  }
  
  fromObject(obj){
    this.type = obj.type;
    this.text = obj.text;
    this.attr = obj.attr;
    for(let o in obj.children){
      this.children+=new Element().fromObject(o);
    }
    return this;
  }
  
  addAttr(type, value){
    this.attr.push([type, value]);
    return this;
  }
  
  addChild(element){
    this.children.push(element);
    return this;
  }
  
  addText(text){
    this.text+=text;
    return this;
  }
  
  appendTo(page){
    let element = this.getSelf();
    page.appendChild(element);
    this.children.forEach((x)=>{
      x.appendTo(element);
    });
  }
  
  getSelf(){
    let element = document.createElement(this.type);
    if(this.text!=="") element.textContent=this.text;
    this.attr.forEach((x)=>{
      const att = document.createAttribute(x[0]);
      att.value = x[1];
      element.setAttributeNode(att);
    });
    return element;
  }
}

function homeDescriptionText(){
  let base = new Element("div");
  base.addChild(new Element("h1").addText("This is the homepage!!!"));
  base.addChild(new Element("p").addText("This is the description...\n I hope you like it <3"));
  return base;
}

function setupHome(){
  let baseElement = new Element("div").addAttr("class", "inherit-all");
  baseElement.addChild(new Element("div").addAttr("class", "page-item").addChild(homeDescriptionText()));
  baseElement.addChild(new Element("div").addAttr("class", "page-item").addText("Goodbye World"));
  return baseElement;
}

function handleLibsList(sidebar){
  for(const [key, o] of Object.entries(docus.lib)){
    let pageaddr = "#docs@"+o.abrv;
    sidebar.addChild(new Element("div").addChild(
      new Element("a")
        .addAttr("class", "lc-item")
        .addAttr("href", pageaddr)
        .addText(o.name)
    ));
  }
}

function handleLangList(sidebar){
  for(const [key, o] of Object.entries(docus.lang)){
    let pageaddr = "#docs@"+o.abrv;
    sidebar.addChild(new Element("div").addChild(
      new Element("a")
        .addAttr("class", "lc-item")
        .addAttr("href", pageaddr)
        .addText(o.name)
    ));
  }
}

function docsList(sidebar){
  sidebar.addChild(new Element("div").addAttr("class", "topic").addText("Libraries"));
  handleLibsList(sidebar);
  
  sidebar.addChild(new Element("div").addAttr("class", "topic").addText("Language"));
  handleLangList(sidebar);
}

function makeDocPage(key, o, left_column){
  let baseElement = new Element("div").addAttr("class", "inherit-all").addAttr("style","align-items: stretch;");
  baseElement.addChild(left_column);
  let right_column = new Element("div").addAttr("class", "right-column");
  baseElement.addChild(right_column);
  right_column.addChild(new Element().fromObject(o.page));
  pages.set("#docs@"+o.abrv, baseElement);
}

function docsPages(left_column){
  for(const [key, o] of Object.entries(docus.lib)){
    makeDocPage(key, o, left_column);
  }
  for(const [key, o] of Object.entries(docus.lang)){
    makeDocPage(key, o, left_column);
  }
}

function setupDocs(){
  let baseElement = new Element("div").addAttr("class", "inherit-all").addAttr("style","align-items: stretch;");
  let left_column = new Element("div").addAttr("class", "left-column");
  docsList(left_column);
  baseElement.addChild(left_column);
  let right_column = new Element("div").addAttr("class", "right-column");
  baseElement.addChild(right_column);
  docsPages(left_column);
  return baseElement;
}

function setupPages(){
  pages.set("#home", setupHome());
  pages.set("#docs", setupDocs());
}

setupPages();
clearPage();
loadPage(window.location.href.substr(window.location.href.indexOf('#')));

function selectItem(s){
  Array.from(menuElements).forEach((x, i)=>{
    x.classList.remove("active");
  });
  let x = document.getElementById(s);
  x.classList.add("active");
}

window.selectItem = selectItem;

function clearPage(){
  while(content.firstChild){
    content.removeChild(content.firstChild);
  }
}

function loadPage(page){
  pages.get(page).appendTo(content);
}

function findPage(){
  console.log("[TODO] Make Search Bar Work");
}

searchbar.addEventListener("keypress", function(event) {
  if(event.key === "Enter"){
    event.preventDefault();
    let input = searchbar.childNodes[1];
    findPage(input.value);
    console.log(input.value);
    input.value = "";
  }
});

function locationHashChanged(){
  clearPage();
  loadPage(location.hash);
}

window.onhashchange = locationHashChanged;