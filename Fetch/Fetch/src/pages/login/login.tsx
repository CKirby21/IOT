import './login.css';
import React from 'react';
import { IonContent, IonHeader, IonItem, IonLabel, IonList, IonPage, IonTitle, IonToolbar } from '@ionic/react';

const Login: React.FC = () => {
  return(
    <IonPage>
      <IonContent>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Login</IonTitle>
        </IonToolbar>
      </IonHeader>

        <IonList className="listwrapper">
          <IonItem routerLink="home">
            <IonLabel>Home</IonLabel>
          </IonItem>
          <IonItem routerLink="login">
            <IonLabel>Login</IonLabel>
          </IonItem>
          <IonItem routerLink="register">
            <IonLabel>Register</IonLabel>
          </IonItem>
        </IonList>
      </IonContent>
    </IonPage>
  );
};

export default Login;