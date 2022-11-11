import './login.css';
import React from 'react';
import { IonContent, IonHeader, IonItem, IonLabel, IonList, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import Header from "../../components/header/header";

const Login: React.FC = () => {
  return(
    <IonPage>
      <IonContent>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Login</IonTitle>
        </IonToolbar>
      </IonHeader>
        <Header />
      </IonContent>
    </IonPage>
  );
};

export default Login;