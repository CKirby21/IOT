import "./header.css";
import { IonItem, IonLabel, IonList } from "@ionic/react";

const Header: React.FC = () => {
  return (
    <IonList className="listwrapper">
      <IonItem routerLink="login">
        <IonLabel>Login</IonLabel>
      </IonItem>
      <IonItem routerLink="register">
        <IonLabel>Register</IonLabel>
      </IonItem>
    </IonList>
  );
};

export default Header;
